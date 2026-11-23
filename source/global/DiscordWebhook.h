#pragma once

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QVector>
#include <QPair>
#include <QJsonArray>
#include <functional>

class QNetworkAccessManager;

// Sends messages to a Discord channel through a webhook address, with Qt Network only, so it
// works the same on Windows, Linux and macOS. Requests are asynchronous: the interface keeps
// working while a message is sent, and no failure can end the program. The callback is called
// exactly once on the GUI thread with a short explanation for the player.
class DiscordWebhook : public QObject
{
    Q_OBJECT
public:
    using Callback = std::function<void(bool ok, const QString &message)>;

    static DiscordWebhook *get();

    // Accepts https://discord.com/api/webhooks/<id>/<token> (also discordapp.com and the
    // ptb/canary subdomains). On failure returns false and explains the problem.
    static bool isValidUrl(const QString &url, QString *problem=nullptr);
    // Address shortened for display, without the secret token.
    static QString maskedUrl(const QString &url);

    // files: (file name, PNG bytes); Discord accepts at most 10 per message. embeds: optional
    // Discord embed objects (coloured cards with fields), sent with the message.
    void send(const QString &url, const QString &text, const QVector<QPair<QString,QByteArray>> &files, Callback done,
              const QJsonArray &embeds = QJsonArray());

private:
    explicit DiscordWebhook(QObject *parent=nullptr);
    QNetworkAccessManager *network;
};
