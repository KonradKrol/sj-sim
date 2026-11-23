#include "DiscordWebhook.h"

#include <QCoreApplication>
#include <QHttpMultiPart>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QUrl>
#include <QtMath>

namespace {
const QRegularExpression webhookPath(QStringLiteral("^/api(/v\\d+)?/webhooks/(\\d+)/([A-Za-z0-9_\\-]+)/?$"));
// Discord limits message text to 2000 characters.
constexpr int textLimit=2000;

QString explain(QNetworkReply *reply)
{
    const int status=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(status==401 || status==403 || status==404)
        return QObject::tr("Discord nie zna tego webhooka. Mógł zostać usunięty. Utwórz nowy w ustawieniach kanału Discord i wklej jego adres w ustawieniach Sj.Sima.");
    if(status==429) {
        const double wait=QJsonDocument::fromJson(reply->readAll()).object().value("retry_after").toDouble();
        return wait>0?QObject::tr("Discord chwilowo ogranicza liczbę wiadomości. Spróbuj ponownie za %1 s.").arg(qCeil(wait))
                     :QObject::tr("Discord chwilowo ogranicza liczbę wiadomości. Spróbuj ponownie za chwilę.");
    }
    if(status==413)return QObject::tr("Obraz jest za duży dla Discorda. Wybierz mniej miejsc.");
    if(status>=400 && status<500)return QObject::tr("Discord odrzucił wiadomość (kod %1).").arg(status);
    if(status>=500)return QObject::tr("Discord ma chwilowe problemy (kod %1). Spróbuj ponownie później.").arg(status);
    if(reply->error()==QNetworkReply::OperationCanceledError || reply->error()==QNetworkReply::TimeoutError)
        return QObject::tr("Discord nie odpowiedział w ciągu 20 sekund. Sprawdź połączenie z internetem.");
    return QObject::tr("Nie udało się połączyć z Discordem. Sprawdź połączenie z internetem.");
}
}

DiscordWebhook *DiscordWebhook::get()
{
    static DiscordWebhook *instance=new DiscordWebhook(QCoreApplication::instance());
    return instance;
}

DiscordWebhook::DiscordWebhook(QObject *parent) : QObject(parent), network(new QNetworkAccessManager(this)) {}

bool DiscordWebhook::isValidUrl(const QString &text, QString *problem)
{
    const auto fail=[problem](const QString &message){if(problem)*problem=message;return false;};
    const QString trimmed=text.trimmed();
    if(trimmed.isEmpty())return fail(QObject::tr("Wklej adres webhooka z Discorda."));
    const QUrl url(trimmed,QUrl::StrictMode);
    if(!url.isValid() || url.scheme()!="https")return fail(QObject::tr("Adres musi zaczynać się od https://discord.com/api/webhooks/."));
    const QString host=url.host().toLower();
    const bool discordHost=host=="discord.com" || host=="discordapp.com" || host=="ptb.discord.com" || host=="canary.discord.com";
    if(!discordHost || !webhookPath.match(url.path()).hasMatch())
        return fail(QObject::tr("To nie jest adres webhooka Discorda. Skopiuj go w Discordzie: Edytuj kanał → Integracje → Webhooki → Kopiuj adres URL webhooka."));
    if(problem)problem->clear();
    return true;
}

QString DiscordWebhook::maskedUrl(const QString &text)
{
    const QUrl url(text.trimmed());
    const auto match=webhookPath.match(url.path());
    if(!match.hasMatch())return {};
    return QStringLiteral("%1/…/%2/••••").arg(url.host(),match.captured(2));
}

void DiscordWebhook::send(const QString &urlText,const QString &text,const QVector<QPair<QString,QByteArray>> &files,Callback done,const QJsonArray &embeds)
{
    QString problem;
    if(!isValidUrl(urlText,&problem)){done(false,problem);return;}
    QUrl url(urlText.trimmed());
    // wait=true: Discord confirms delivery in the response instead of only accepting it.
    url.setQuery(QStringLiteral("wait=true"));

    // Mentions are disabled so that a name in the results can never ping anybody.
    QJsonObject payload{{"content",text.left(textLimit)},{"allowed_mentions",QJsonObject{{"parse",QJsonArray{}}}}};
    if(!embeds.isEmpty())payload["embeds"]=embeds;
    auto *body=new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart json;
    json.setHeader(QNetworkRequest::ContentDispositionHeader,QStringLiteral("form-data; name=\"payload_json\""));
    json.setHeader(QNetworkRequest::ContentTypeHeader,QStringLiteral("application/json"));
    json.setBody(QJsonDocument(payload).toJson(QJsonDocument::Compact));
    body->append(json);
    for(int i=0;i<files.size() && i<10;++i){
        QString name=files[i].first;name.remove(QLatin1Char('"'));
        QHttpPart image;
        image.setHeader(QNetworkRequest::ContentDispositionHeader,QStringLiteral("form-data; name=\"files[%1]\"; filename=\"%2\"").arg(i).arg(name));
        image.setHeader(QNetworkRequest::ContentTypeHeader,QStringLiteral("image/png"));
        image.setBody(files[i].second);
        body->append(image);
    }
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader,QStringLiteral("Sj.Sim"));
    request.setTransferTimeout(20000);
    QNetworkReply *reply=network->post(request,body);
    body->setParent(reply);
    connect(reply,&QNetworkReply::finished,this,[reply,done]{
        const int status=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const bool ok=reply->error()==QNetworkReply::NoError && status>=200 && status<300;
        const QString message=ok?QObject::tr("Wysłano na Discorda."):explain(reply);
        reply->deleteLater();
        done(ok,message);
    });
}
