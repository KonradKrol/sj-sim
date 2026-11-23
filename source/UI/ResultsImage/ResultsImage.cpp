#include "../../global/AppPaths.h"
#include "ResultsImage.h"

#include "../../competitions/CompetitionInfo.h"
#include "../../competitions/CompetitionResults.h"
#include "../../competitions/CompetitionSingleResult.h"
#include "../../seasons/Classification.h"
#include "../../seasons/ClassificationSingleResult.h"
#include "../../simulator/Hill.h"
#include "../../simulator/Jumper.h"
#include "../../simulator/Team.h"
#include "../../simulator/JumpData.h"
#include "../../global/CountryCodes.h"
#include "../../global/GlobalDatabase.h"
#include "../../global/UserDataPaths.h"
#include "../../global/DiscordWebhook.h"
#include "../../global/GlobalAppSettings.h"
#include "../../global/JumpDataInfoChoice.h"
#include "../Notice.h"
#include <QJsonArray>
#include <QJsonObject>
#include "../mainwindow.h"

#include <QBuffer>
#include <QClipboard>
#include <QDateTime>
#include <QPointer>
#include <QSaveFile>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QImageReader>
#include <QInputDialog>
#include <QLocale>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QRegularExpression>
#include <QtMath>
#include <algorithm>
#include <array>
#include <functional>
#include <memory>

namespace {
// Alpine palette shared with the light "Alpejski" theme of the interface.
const QColor pageColor("#f1f0e9");
const QColor cardColor("#ffffff");
const QColor stripeColor("#f5f6f3");
const QColor headerColor("#142e42");
const QColor ridgeColor("#1c3d56");
const QColor ridgeFarColor("#18354b");
const QColor signalColor("#d8ee85");
const QColor snowColor("#f1f6ff");
const QColor headerMutedColor("#b5c8df");
const QColor inkColor("#112b49");
const QColor mutedColor("#51657c");
const QColor dividerColor("#d9dfe4");
const QColor goldColor("#d9a520");
const QColor silverColor("#9aa6b2");
const QColor bronzeColor("#b4733c");

QString number(double value) {return QLocale().toString(value,'f',1);}

QString countryName(const QString &code)
{
    if(auto *country=GlobalDatabase::get()->getCountryByAlpha3(code))return country->getName();
    return code;
}

QImage flagImage(const QString &country,QSize size)
{
    if(country.isEmpty())return {};
    QImageReader reader(AppPaths::flagFile(country));
    reader.setScaledSize(size);
    return reader.read();
}

QFont font(const QString &family,qreal pixelSize,QFont::Weight weight)
{
    QFont value(family);
    value.setPixelSize(qRound(pixelSize));
    value.setWeight(weight);
    value.setStyleStrategy(QFont::PreferAntialias);
    return value;
}

void drawFlag(QPainter &painter,const QString &country,const QRectF &target,qreal ratio)
{
    const QImage image=flagImage(country,(target.size()*ratio).toSize());
    if(image.isNull())return;
    QPainterPath clip;clip.addRoundedRect(target,3,3);
    painter.save();painter.setClipPath(clip);painter.drawImage(target,image);painter.restore();
    painter.setPen(QPen(QColor(0,0,0,40),1));painter.setBrush(Qt::NoBrush);painter.drawRoundedRect(target,3,3);
}

QString seriesTitle(CompetitionInfo *competition)
{
    QString text=competition->getLongSerieTypeText();
    if(auto *rules=competition->getRulesPointer())
        text+=rules->getCompetitionType()==CompetitionRules::Team?QObject::tr(" · drużynowy"):QObject::tr(" · indywidualny");
    return text;
}

QString jumpText(const JumpData &jump)
{
    if(jump.getDSQ())return QObject::tr("DSQ");
    if(jump.getDNS())return QObject::tr("DNS");
    return QObject::tr("%1 m").arg(number(jump.getDistance()));
}

QString safeFileName(QString text)
{
    text.replace(QRegularExpression(QStringLiteral("[\\\\/:*?\"<>|]+")),QStringLiteral(" "));
    return text.simplified();
}

constexpr int maxSeparateImages=3;

struct Choice
{
    int limit=0;          // places shown, 0 = all
    int perImage=30;      // places on one image
    bool accepted=false;
};

// Asks how many places to show and, for all places, how many fit on one image: 30 reads well
// in a chat window, 50 gives fewer files.
Choice askChoice(QWidget *parent,int available)
{
    QStringList options;QVector<Choice> choices;
    for(int limit:{10,30})if(available>limit){options<<QObject::tr("%1 pierwszych miejsc · jeden obraz").arg(limit);choices.append({limit,limit,true});}
    if(available<=30){options<<QObject::tr("Wszystkie miejsca (%1) · jeden obraz").arg(available);choices.append({0,30,true});}
    else for(int perImage:{30,50}){
        const int images=(available+perImage-1)/perImage;
        options<<(images>1?QObject::tr("Wszystkie miejsca (%1) · po %2 na obrazie, obrazów: %3").arg(available).arg(perImage).arg(images)
                          :QObject::tr("Wszystkie miejsca (%1) · jeden obraz").arg(available));
        choices.append({0,perImage,true});
    }
    int preferred=options.size()-1;
    for(int i=0;i<choices.size();++i)if(choices[i].limit==30)preferred=i;
    bool accepted=false;
    const QString selected=QInputDialog::getItem(parent,QObject::tr("Obraz wyników"),
        QObject::tr("Ile miejsc pokazać? Krótsza lista jest czytelniejsza w czacie. Pełne wyniki można podzielić na kilka obrazów; więcej niż trzy zapiszą się jako archiwum ZIP."),
        options,preferred,false,&accepted);
    if(!accepted)return {};
    return choices.value(options.indexOf(selected));
}

// Minimal ZIP writer with stored entries (PNG is already compressed). It needs no extra
// library, so it works the same on every platform.
quint32 crc32(const QByteArray &data)
{
    static const auto table=[]{std::array<quint32,256> t{};for(quint32 i=0;i<256;++i){quint32 c=i;for(int k=0;k<8;++k)c=(c&1)?0xEDB88320u^(c>>1):c>>1;t[i]=c;}return t;}();
    quint32 crc=0xFFFFFFFFu;
    for(const char byte:data)crc=table[(crc^quint8(byte))&0xFF]^(crc>>8);
    return crc^0xFFFFFFFFu;
}

QByteArray zipArchive(const QVector<QPair<QString,QByteArray>> &files)
{
    QByteArray archive,directory;
    const QDateTime now=QDateTime::currentDateTime();
    const quint16 dosTime=quint16((now.time().hour()<<11)|(now.time().minute()<<5)|(now.time().second()/2));
    const quint16 dosDate=quint16(((qMax(1980,now.date().year())-1980)<<9)|(now.date().month()<<5)|now.date().day());
    const auto put16=[](QByteArray &out,quint16 v){out.append(char(v&0xFF));out.append(char(v>>8));};
    const auto put32=[&](QByteArray &out,quint32 v){put16(out,quint16(v&0xFFFF));put16(out,quint16(v>>16));};
    for(const auto &file:files){
        const QByteArray name=file.first.toUtf8();const quint32 crc=crc32(file.second);const quint32 size=quint32(file.second.size());
        const quint32 offset=quint32(archive.size());
        // Local header. Flag 0x0800: file names are UTF-8.
        put32(archive,0x04034b50);put16(archive,20);put16(archive,0x0800);put16(archive,0);put16(archive,dosTime);put16(archive,dosDate);
        put32(archive,crc);put32(archive,size);put32(archive,size);put16(archive,quint16(name.size()));put16(archive,0);
        archive.append(name);archive.append(file.second);
        // Central directory entry.
        put32(directory,0x02014b50);put16(directory,20);put16(directory,20);put16(directory,0x0800);put16(directory,0);put16(directory,dosTime);put16(directory,dosDate);
        put32(directory,crc);put32(directory,size);put32(directory,size);put16(directory,quint16(name.size()));put16(directory,0);put16(directory,0);
        put16(directory,0);put16(directory,0);put32(directory,0);put32(directory,offset);directory.append(name);
    }
    const quint32 directoryOffset=quint32(archive.size());
    archive.append(directory);
    put32(archive,0x06054b50);put16(archive,0);put16(archive,0);put16(archive,quint16(files.size()));put16(archive,quint16(files.size()));
    put32(archive,quint32(directory.size()));put32(archive,directoryOffset);put16(archive,0);
    return archive;
}

QByteArray pngBytes(const QImage &image)
{
    QByteArray bytes;QBuffer buffer(&bytes);buffer.open(QIODevice::WriteOnly);image.save(&buffer,"PNG");return bytes;
}

QString numbered(const QString &base,int index,int count)
{
    return count==1?base:QObject::tr("%1 (%2 z %3)").arg(base).arg(index+1).arg(count);
}

bool saveImages(QWidget *parent,const QVector<QImage> &images,const QString &suggestedName)
{
    const QString folder=UserDataPaths::path(QStringLiteral("Exports"));
    QDir().mkpath(folder);
    const QString base=safeFileName(suggestedName);
    const bool archive=images.size()>maxSeparateImages;
    const QString extension=archive?QStringLiteral("zip"):QStringLiteral("png");
    QString path=QFileDialog::getSaveFileName(parent,archive?QObject::tr("Zapisz obrazy wyników jako archiwum ZIP"):QObject::tr("Zapisz obraz wyników"),
        QDir(folder).filePath(base+"."+extension),archive?QObject::tr("Archiwum ZIP (*.zip)"):QObject::tr("Obraz PNG (*.png)"));
    if(path.isEmpty())return false;
    if(QFileInfo(path).suffix().compare(extension,Qt::CaseInsensitive)!=0)path+="."+extension;
    const QFileInfo target(path);
    const QString chosenBase=target.completeBaseName();
    QString failed;
    if(archive){
        QVector<QPair<QString,QByteArray>> files;
        for(int i=0;i<images.size();++i)files.append({numbered(chosenBase,i,images.size())+QStringLiteral(".png"),pngBytes(images[i])});
        QSaveFile file(path);
        if(!file.open(QIODevice::WriteOnly) || file.write(zipArchive(files))<0 || !file.commit())failed=path;
    } else {
        for(int i=0;i<images.size() && failed.isEmpty();++i){
            const QString name=images.size()==1?path:target.dir().filePath(numbered(chosenBase,i,images.size())+QStringLiteral(".png"));
            if(images[i].isNull() || !images[i].save(name,"PNG"))failed=name;
        }
    }
    if(!failed.isEmpty()){
        QMessageBox::warning(parent,QObject::tr("Nie zapisano obrazu"),QObject::tr("Nie udało się zapisać pliku %1. Wybierz inny folder.").arg(QDir::toNativeSeparators(failed)));
        return false;
    }
    if(images.size()==1){
        if(auto *clipboard=QGuiApplication::clipboard())clipboard->setImage(images.first());
        QMessageBox::information(parent,QObject::tr("Zapisano obraz wyników"),
            QObject::tr("Obraz zapisano w pliku:\n%1\n\nJest też w schowku, więc możesz go od razu wkleić, na przykład na Discordzie.").arg(QDir::toNativeSeparators(path)));
    } else if(archive) {
        QMessageBox::information(parent,QObject::tr("Zapisano obrazy wyników"),
            QObject::tr("Liczba obrazów w archiwum: %1.\n%2").arg(images.size()).arg(QDir::toNativeSeparators(path)));
    } else {
        QMessageBox::information(parent,QObject::tr("Zapisano obrazy wyników"),
            QObject::tr("Liczba zapisanych obrazów: %1, w folderze:\n%2").arg(images.size()).arg(QDir::toNativeSeparators(target.absolutePath())));
    }
    return true;
}

QString caption(const ResultsCard &card,const QString &title=QString())
{
    QStringList parts{QStringLiteral("**%1**").arg(title.isEmpty()?card.title:title),card.eyebrow};
    if(!card.subtitle.isEmpty())parts<<card.subtitle;
    return parts.join(QStringLiteral(" · "));
}

void reportDiscordResult(const QPointer<QWidget> &owner,bool ok,const QString &message);

void publishImages(QWidget *parent,const QVector<QImage> &images,const ResultsCard &card,const QString &baseName,const QString &title=QString())
{
    // Discord accepts up to 10 files in one message; larger sets go as several messages.
    struct Batch{QString text;QVector<QPair<QString,QByteArray>> files;};
    QVector<Batch> batches;
    for(int i=0;i<images.size();++i){
        if(i%10==0)batches.append({batches.isEmpty()?caption(card,title):QString(),{}});
        batches.last().files.append({safeFileName(numbered(baseName,i,images.size()))+QStringLiteral(".png"),pngBytes(images[i])});
    }
    const QPointer<QWidget> owner(parent);
    const QString url=GlobalAppSettings::get()->getDiscordWebhookUrl();
    auto sendNext=std::make_shared<std::function<void(int)>>();
    *sendNext=[owner,batches,url,sendNext](int index){
        DiscordWebhook::get()->send(url,batches[index].text,batches[index].files,
            [owner,batches,sendNext,index](bool ok,const QString &message){
                if(ok && index+1<batches.size()){(*sendNext)(index+1);return;}
                reportDiscordResult(owner,ok,message);
                *sendNext=nullptr;
            });
    };
    (*sendNext)(0);
}

// Discord flag emoji (":flag_no:") for a FIS or ISO country code, or nothing when unknown.
QString discordFlag(const QString &code)
{
    Country *country=GlobalDatabase::get()->getCountryByAlpha3(code);
    if(!country)country=GlobalDatabase::get()->getCountryByAlpha3(CountryCodes::toIso(code));
    if(!country || country->getAlpha2().isEmpty())return {};
    return QStringLiteral(":flag_%1:").arg(country->getAlpha2().toLower());
}

// Same layout as the jump message of earlier Sj.Sim versions: a coloured card with the athlete
// and flag in the title, the hill and event in the description and one field per detail chosen
// in "Informacje o skoku".
QJsonObject jumpEmbed(JumpData *jump)
{
    const auto number=[](double value,int decimals){return QString::number(value,'f',decimals);};
    QJsonArray fields;
    const auto field=[&](const QString &name,const QString &value,bool inline_){fields.append(QJsonObject{{"name",name},{"value",value},{"inline",inline_}});};
    Jumper *jumper=jump->getJumper();
    CompetitionInfo *competition=jump->getCompetition();
    const bool competitionJump=!jump->getInSingleJumps() && competition;
    const bool team=competitionJump && competition->getRulesPointer()->getCompetitionType()==CompetitionRules::Team;
    int round=0;
    if(auto *result=jump->getSingleResult())
        for(int i=0;i<result->getJumpsReference().size();++i)if(&result->getJumpsReference()[i]==jump){round=i+1;break;}
    Hill *hill=competitionJump?competition->getHill():jump->getHill();
    const QString hillText=hill?QStringLiteral("%1 %2 HS%3").arg(hill->getName(),discordFlag(hill->getCountryCode())).arg(hill->getHSPoint()):QString();
    QString description;
    if(competitionJump)
        description=hillText+QStringLiteral(" (%1) ").arg(competition->getLongSerieTypeText())+(team?QObject::tr("drużynowy"):QObject::tr("indywidualny"))+QObject::tr(" - Skok %1").arg(round);
    else description=hillText+QObject::tr(" (tryb pojedynczych skoków)");

    const auto *rules=competition?competition->getRulesPointer():nullptr;
    const bool gateComp=!rules || rules->getHasGateCompensations();
    const bool windComp=!rules || rules->getHasWindCompensations();
    const bool judges=!rules || rules->getHasJudgesPoints();
    const JumpDataInfoChoice &c=GlobalAppSettings::get()->getJumpDataInfoChoiceReference();
    if(jump->getDNS() || jump->getDSQ())field(QObject::tr("Odległość"),jump->getDNS()?QStringLiteral("DNS"):QStringLiteral("DSQ"),true);
    else {
        if(c.getDistance())field(QObject::tr("Odległość"),number(jump->getDistance(),1),true);
        if(c.getPoints())field(QObject::tr("Punkty"),number(jump->getPoints(),1),true);
        if(c.getGate())field(QObject::tr("Belka"),QString::number(jump->getGate()),true);
        if(c.getAveragedWind())field(QObject::tr("Uśredniony wiatr"),number(jump->getAveragedWind(),1),true);
        if(c.getGateCompensation() && gateComp)field(QObject::tr("Rekompensata za belkę"),number(jump->getGateCompensation(),1),false);
        if(c.getWindCompensation() && windComp)field(QObject::tr("Rekompensata za wiatr"),number(jump->getWindCompensation(),1),true);
        if(c.getTotalCompensation() && (gateComp || windComp))field(QObject::tr("Łączna rekompensata"),number(jump->getTotalCompensation(),1),true);
        if(c.getJudges() && judges)field(QObject::tr("Noty sędziowskie"),jump->getJudgesText(),false);
        if(c.getJudgesPoints() && judges)field(QObject::tr("Punkty od sędziów"),number(jump->getJudgesPoints(),1),true);
        if(c.getLandingType())field(QObject::tr("Rodzaj lądowania"),jump->getLanding().getTextLandingType(),true);
        if(c.getSpecificWind())field(QObject::tr("Wiatr przy skoku"),jump->getWindsText(),false);
    }
    if(c.getPositionAfterJump() && competitionJump){
        CompetitionSingleResult *result=team?competition->getResultsReference().getResultOfTeam(Team::getTeamByCountryCode(&competition->getTeamsReference(),jumper->getCountryCode()))
                                            :competition->getResultsReference().getResultOfIndividualJumper(jumper);
        if(result)field(QObject::tr("Miejsce po skoku"),QString::number(result->getPosition()),true);
    }
    if(c.getJumpPositionInRound() && competitionJump && !team)field(QObject::tr("Pozycja noty w serii"),QString::number(jump->getPositionInRound()),true);
    if(c.getInrunSnow())field(QObject::tr("Śnieg/deszcz przy skoku"),number(jump->getSimulationDataReference().getInrunSnow(),1),true);

    return QJsonObject{
        {"color",0x38B0DE},
        {"title",QStringLiteral("**%1 %2**").arg(jumper->getNameAndSurname(),discordFlag(jumper->getCountryCode()))},
        {"description",description},
        {"fields",fields},
        {"footer",QJsonObject{{"text",QObject::tr("Wiadomość wysłana z poziomu Sj.Sim ")+appVersion+QStringLiteral("\nhttps://github.com/KonradKrol/sj-sim")}}}
    };
}

void reportDiscordResult(const QPointer<QWidget> &owner,bool ok,const QString &message)
{
    if(ok)Notice::show(message);
    else QMessageBox::warning(owner.data(),QObject::tr("Nie wysłano na Discorda"),message);
}

bool discordReady(QWidget *parent)
{
    QString problem;
    if(DiscordWebhook::isValidUrl(GlobalAppSettings::get()->getDiscordWebhookUrl(),&problem))return true;
    QMessageBox::information(parent,QObject::tr("Discord nie jest ustawiony"),
        QObject::tr("Aby publikować wyniki, wklej adres webhooka kanału w Ustawieniach, w sekcji „Discord”.\n\n%1").arg(problem));
    return false;
}
}

namespace ResultsImage {

ResultsCard fromCompetition(CompetitionInfo *competition,int limit)
{
    ResultsCard card;
    if(!competition)return card;
    card.eyebrow=seriesTitle(competition);
    if(auto *hill=competition->getHill()){
        card.title=QObject::tr("%1 HS%2").arg(hill->getName()).arg(hill->getHSPoint());
        card.titleCountry=hill->getCountryCode();
    }
    QStringList subtitle;
    if(competition->getScheduledDate().isValid())subtitle<<QLocale().toString(competition->getScheduledDate(),QStringLiteral("d MMMM yyyy"));
    subtitle<<(competition->getPlayed()?QObject::tr("Wyniki końcowe"):QObject::tr("Wyniki w trakcie konkursu"));
    card.subtitle=subtitle.join(QStringLiteral(" · "));
    card.totalColumn=QObject::tr("Nota");

    QVector<CompetitionSingleResult *> results;
    for(auto &result:competition->getResultsReference().getResultsReference())results.append(&result);
    std::stable_sort(results.begin(),results.end(),[](auto *left,auto *right){return left->getPosition()<right->getPosition();});
    const bool team=competition->getRulesPointer() && competition->getRulesPointer()->getCompetitionType()==CompetitionRules::Team;
    int rounds=0;
    if(!team)for(auto *result:results)rounds=std::max(rounds,int(result->getJumpsReference().size()));
    for(int round=1;round<=rounds;++round)card.valueColumns<<QObject::tr("%1. seria").arg(round);

    for(auto *result:results){
        if(limit>0 && card.rows.size()>=limit){++card.hiddenRows;continue;}
        ResultsCard::Row row;row.position=result->getPosition();row.total=number(result->getPointsSum());
        if(team && result->getTeam()){
            row.country=result->getTeam()->getCountryCode();row.name=countryName(row.country);
            QStringList members;for(auto *jumper:result->getTeam()->getJumpersReference())if(jumper)members<<jumper->getSurname();
            row.detail=members.join(QStringLiteral(", "));
        } else if(result->getJumper()){
            row.country=result->getJumper()->getCountryCode();row.name=result->getJumper()->getNameAndSurname();row.detail=row.country;
            for(const auto &jump:result->getJumpsReference())row.values<<jumpText(jump);
        } else continue;
        card.rows.append(row);
    }
    return card;
}

ResultsCard fromClassification(Classification *classification,int limit)
{
    ResultsCard card;
    if(!classification)return card;
    const bool team=classification->getClassificationType()==Classification::Team;
    card.eyebrow=team?QObject::tr("Klasyfikacja drużynowa"):QObject::tr("Klasyfikacja indywidualna");
    card.title=classification->getName();
    card.subtitle=QObject::tr("Aktualny stan");
    card.totalColumn=QObject::tr("Punkty");
    QVector<ClassificationSingleResult *> results;
    for(auto *result:classification->getResultsReference())if(result)results.append(result);
    std::stable_sort(results.begin(),results.end(),[](auto *left,auto *right){return left->getPosition()<right->getPosition();});
    for(auto *result:results){
        if(limit>0 && card.rows.size()>=limit){++card.hiddenRows;continue;}
        ResultsCard::Row row;row.position=result->getPosition();row.total=number(result->getPointsSum());
        if(team){row.country=result->getTeamCode();row.name=countryName(row.country);}
        else if(result->getJumper()){row.country=result->getJumper()->getCountryCode();row.name=result->getJumper()->getNameAndSurname();row.detail=row.country;}
        else continue;
        card.rows.append(row);
    }
    return card;
}

QImage render(const ResultsCard &card,qreal ratio)
{
    const qreal width=1200, margin=32, headerHeight=card.subtitle.isEmpty()?150:176;
    const qreal tableTop=headerHeight+24, columnHeader=40, rowHeight=card.rows.isEmpty()?0:46;
    const qreal hiddenNote=card.hiddenRows>0?40:0;
    const qreal emptyNote=card.rows.isEmpty()?64:0;
    const qreal tableHeight=columnHeader+rowHeight*card.rows.size()+hiddenNote+emptyNote+12;
    const qreal height=tableTop+tableHeight+68;

    QImage image(QSize(qCeil(width*ratio),qCeil(height*ratio)),QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(ratio);
    image.fill(pageColor);
    QPainter painter(&image);
    painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);

    // Header band with a mountain ridge, the alpine signature of the interface.
    painter.fillRect(QRectF(0,0,width,headerHeight),headerColor);
    QPainterPath farRidge;farRidge.moveTo(width*.46,headerHeight);farRidge.lineTo(width*.62,headerHeight*.38);farRidge.lineTo(width*.71,headerHeight*.62);
    farRidge.lineTo(width*.83,headerHeight*.18);farRidge.lineTo(width,headerHeight*.52);farRidge.lineTo(width,headerHeight);farRidge.closeSubpath();
    painter.fillPath(farRidge,ridgeFarColor);
    QPainterPath nearRidge;nearRidge.moveTo(width*.58,headerHeight);nearRidge.lineTo(width*.76,headerHeight*.46);nearRidge.lineTo(width*.86,headerHeight*.70);
    nearRidge.lineTo(width*.94,headerHeight*.40);nearRidge.lineTo(width,headerHeight*.58);nearRidge.lineTo(width,headerHeight);nearRidge.closeSubpath();
    painter.fillPath(nearRidge,ridgeColor);
    painter.fillRect(QRectF(0,headerHeight-5,width,5),signalColor);

    qreal textLeft=margin;
    if(!card.titleCountry.isEmpty()){drawFlag(painter,card.titleCountry,QRectF(margin,68,54,36),ratio);textLeft+=70;}
    painter.setPen(signalColor);painter.setFont(font(QStringLiteral("Barlow"),16,QFont::DemiBold));
    painter.drawText(QRectF(margin,26,width-2*margin,24),Qt::AlignLeft|Qt::AlignVCenter,card.eyebrow.toUpper());
    painter.setPen(snowColor);painter.setFont(font(QStringLiteral("Barlow Condensed"),46,QFont::ExtraBold));
    const QFontMetricsF titleMetrics(painter.font());
    painter.drawText(QRectF(textLeft,56,width-textLeft-margin,60),Qt::AlignLeft|Qt::AlignVCenter,titleMetrics.elidedText(card.title,Qt::ElideRight,width*.62-textLeft));
    if(!card.subtitle.isEmpty()){
        painter.setPen(headerMutedColor);painter.setFont(font(QStringLiteral("Barlow"),19,QFont::Medium));
        painter.drawText(QRectF(margin,120,width-2*margin,30),Qt::AlignLeft|Qt::AlignVCenter,card.pageNote.isEmpty()?card.subtitle:card.subtitle+QStringLiteral(" · ")+card.pageNote);
    }

    // Results table.
    const QRectF table(margin,tableTop,width-2*margin,tableHeight);
    painter.setPen(Qt::NoPen);painter.setBrush(cardColor);painter.drawRoundedRect(table,14,14);
    const qreal positionWidth=64, flagWidth=48, detailWidth=card.valueColumns.isEmpty()?120:84, totalWidth=118, valueWidth=112;
    const qreal valuesWidth=valueWidth*card.valueColumns.size();
    const qreal left=table.left()+16, right=table.right()-20;
    const qreal nameLeft=left+positionWidth+flagWidth;
    const qreal totalLeft=right-totalWidth, valuesLeft=totalLeft-valuesWidth, detailLeft=valuesLeft-detailWidth-8;
    const bool teamDetails=std::any_of(card.rows.begin(),card.rows.end(),[](const auto &row){return row.detail.contains(',');});

    painter.setFont(font(QStringLiteral("Barlow"),13,QFont::DemiBold));painter.setPen(mutedColor);
    const qreal headY=table.top()+10;
    painter.drawText(QRectF(left,headY,positionWidth-12,22),Qt::AlignRight|Qt::AlignVCenter,QObject::tr("Miejsce").toUpper());
    painter.drawText(QRectF(nameLeft,headY,detailLeft-nameLeft,22),Qt::AlignLeft|Qt::AlignVCenter,(teamDetails?QObject::tr("Drużyna"):QObject::tr("Zawodnik")).toUpper());
    for(int i=0;i<card.valueColumns.size();++i)
        painter.drawText(QRectF(valuesLeft+i*valueWidth,headY,valueWidth-8,22),Qt::AlignRight|Qt::AlignVCenter,card.valueColumns[i].toUpper());
    painter.drawText(QRectF(totalLeft,headY,totalWidth,22),Qt::AlignRight|Qt::AlignVCenter,card.totalColumn.toUpper());
    painter.setPen(QPen(dividerColor,1));painter.drawLine(QPointF(table.left()+12,table.top()+columnHeader),QPointF(table.right()-12,table.top()+columnHeader));

    qreal y=table.top()+columnHeader;
    for(int index=0;index<card.rows.size();++index){
        const auto &row=card.rows[index];
        const QRectF rowRect(table.left()+6,y+2,table.width()-12,rowHeight-4);
        if(index%2==1){painter.setPen(Qt::NoPen);painter.setBrush(stripeColor);painter.drawRoundedRect(rowRect,8,8);}
        const qreal centre=y+rowHeight/2;
        const QColor medal=row.position==1?goldColor:row.position==2?silverColor:row.position==3?bronzeColor:QColor();
        painter.setFont(font(QStringLiteral("Barlow Condensed"),22,QFont::ExtraBold));
        if(medal.isValid()){
            painter.setPen(Qt::NoPen);painter.setBrush(medal);painter.drawEllipse(QPointF(left+positionWidth-30,centre),15,15);
            painter.setPen(Qt::white);painter.drawText(QRectF(left+positionWidth-45,centre-15,30,30),Qt::AlignCenter,QString::number(row.position));
        } else {
            painter.setPen(inkColor);painter.drawText(QRectF(left,centre-15,positionWidth-18,30),Qt::AlignRight|Qt::AlignVCenter,QString::number(row.position));
        }
        drawFlag(painter,row.country,QRectF(left+positionWidth,centre-11,33,22),ratio);
        const bool showDetail=!row.detail.isEmpty();
        painter.setPen(inkColor);painter.setFont(font(QStringLiteral("Barlow"),19,row.position<=3?QFont::DemiBold:QFont::Medium));
        const QFontMetricsF nameMetrics(painter.font());
        if(showDetail && teamDetails){
            painter.drawText(QRectF(nameLeft,y+4,valuesLeft-nameLeft-12,22),Qt::AlignLeft|Qt::AlignVCenter,nameMetrics.elidedText(row.name,Qt::ElideRight,valuesLeft-nameLeft-12));
            painter.setPen(mutedColor);painter.setFont(font(QStringLiteral("Barlow"),14,QFont::Normal));
            const QFontMetricsF detailMetrics(painter.font());
            painter.drawText(QRectF(nameLeft,y+24,valuesLeft-nameLeft-12,18),Qt::AlignLeft|Qt::AlignVCenter,detailMetrics.elidedText(row.detail,Qt::ElideRight,valuesLeft-nameLeft-12));
        } else {
            painter.drawText(QRectF(nameLeft,centre-14,detailLeft-nameLeft-8,28),Qt::AlignLeft|Qt::AlignVCenter,nameMetrics.elidedText(row.name,Qt::ElideRight,detailLeft-nameLeft-8));
            if(showDetail){
                painter.setPen(mutedColor);painter.setFont(font(QStringLiteral("Barlow"),15,QFont::Medium));
                painter.drawText(QRectF(detailLeft,centre-14,detailWidth,28),Qt::AlignLeft|Qt::AlignVCenter,row.detail);
            }
        }
        painter.setFont(font(QStringLiteral("Barlow"),18,QFont::Medium));painter.setPen(mutedColor);
        for(int i=0;i<row.values.size() && i<card.valueColumns.size();++i)
            painter.drawText(QRectF(valuesLeft+i*valueWidth,centre-14,valueWidth-8,28),Qt::AlignRight|Qt::AlignVCenter,row.values[i]);
        painter.setFont(font(QStringLiteral("Barlow Condensed"),23,QFont::ExtraBold));painter.setPen(inkColor);
        painter.drawText(QRectF(totalLeft,centre-15,totalWidth,30),Qt::AlignRight|Qt::AlignVCenter,row.total);
        y+=rowHeight;
    }
    painter.setFont(font(QStringLiteral("Barlow"),15,QFont::Medium));painter.setPen(mutedColor);
    if(card.rows.isEmpty())
        painter.drawText(QRectF(table.left(),y,table.width(),emptyNote),Qt::AlignCenter,QObject::tr("Brak wyników do pokazania."));
    if(card.hiddenRows>0)
        painter.drawText(QRectF(nameLeft,y+6,right-nameLeft,28),Qt::AlignLeft|Qt::AlignVCenter,QObject::tr("Pozostałe miejsca: %1").arg(card.hiddenRows));

    painter.setFont(font(QStringLiteral("Barlow"),14,QFont::Medium));painter.setPen(mutedColor);
    painter.drawText(QRectF(margin,height-52,width-2*margin,32),Qt::AlignLeft|Qt::AlignVCenter,QObject::tr("Sj.Sim %1 · symulacja skoków narciarskich").arg(appVersion));
    painter.end();
    return image;
}

QVector<ResultsCard> pages(const ResultsCard &card,int rowsPerPage)
{
    if(card.rows.size()<=rowsPerPage)return {card};
    QVector<ResultsCard> result;
    const int count=(card.rows.size()+rowsPerPage-1)/rowsPerPage;
    for(int page=0;page<count;++page){
        ResultsCard part=card;
        part.rows=card.rows.mid(page*rowsPerPage,rowsPerPage);
        part.hiddenRows=page==count-1?card.hiddenRows:0;
        part.pageNote=QObject::tr("Miejsca %1–%2 · obraz %3 z %4").arg(part.rows.first().position).arg(part.rows.last().position).arg(page+1).arg(count);
        result.append(part);
    }
    return result;
}
}

namespace {
QVector<QImage> renderAll(const ResultsCard &card,int perImage)
{
    QVector<QImage> images;
    for(const auto &page:ResultsImage::pages(card,perImage))images.append(ResultsImage::render(page));
    return images;
}

QString competitionFileName(CompetitionInfo *competition,const ResultsCard &card)
{
    QString name=card.title+QStringLiteral(" ")+competition->getLongSerieTypeText();
    if(competition->getScheduledDate().isValid())name+=QStringLiteral(" ")+competition->getScheduledDate().toString(Qt::ISODate);
    return name;
}

bool hasCompetitionResults(QWidget *parent,CompetitionInfo *competition)
{
    if(competition && !competition->getResultsReference().getResultsReference().isEmpty())return true;
    QMessageBox::information(parent,QObject::tr("Obraz wyników"),QObject::tr("Ten konkurs nie ma jeszcze wyników."));
    return false;
}

bool hasClassificationResults(QWidget *parent,Classification *classification)
{
    if(classification && !classification->getResultsReference().isEmpty())return true;
    QMessageBox::information(parent,QObject::tr("Obraz wyników"),QObject::tr("Ta klasyfikacja nie ma jeszcze wyników."));
    return false;
}
}

namespace ResultsImage {
bool exportCompetition(QWidget *parent,CompetitionInfo *competition)
{
    if(!hasCompetitionResults(parent,competition))return false;
    const Choice choice=askChoice(parent,competition->getResultsReference().getResultsReference().size());if(!choice.accepted)return false;
    const ResultsCard card=fromCompetition(competition,choice.limit);
    return saveImages(parent,renderAll(card,choice.perImage),competitionFileName(competition,card));
}

bool exportClassification(QWidget *parent,Classification *classification)
{
    if(!hasClassificationResults(parent,classification))return false;
    const Choice choice=askChoice(parent,classification->getResultsReference().size());if(!choice.accepted)return false;
    return saveImages(parent,renderAll(fromClassification(classification,choice.limit),choice.perImage),classification->getName());
}

bool publishCompetition(QWidget *parent,CompetitionInfo *competition)
{
    if(!discordReady(parent) || !hasCompetitionResults(parent,competition))return false;
    const Choice choice=askChoice(parent,competition->getResultsReference().getResultsReference().size());if(!choice.accepted)return false;
    const ResultsCard card=fromCompetition(competition,choice.limit);
    // The hill keeps its flag in the caption, as in the results message of earlier versions.
    QString title=card.title;
    if(auto *hill=competition->getHill())title=QStringLiteral("%1 %2 HS%3").arg(hill->getName(),discordFlag(hill->getCountryCode())).arg(hill->getHSPoint());
    publishImages(parent,renderAll(card,choice.perImage),card,competitionFileName(competition,card),title);
    return true;
}

bool publishClassification(QWidget *parent,Classification *classification)
{
    if(!discordReady(parent) || !hasClassificationResults(parent,classification))return false;
    const Choice choice=askChoice(parent,classification->getResultsReference().size());if(!choice.accepted)return false;
    const ResultsCard card=fromClassification(classification,choice.limit);
    publishImages(parent,renderAll(card,choice.perImage),card,classification->getName());
    return true;
}

bool publishJump(QWidget *parent,JumpData *jump)
{
    if(!jump || !jump->getJumper()){QMessageBox::information(parent,QObject::tr("Discord"),QObject::tr("Nie ma jeszcze skoku do opublikowania."));return false;}
    if(!discordReady(parent))return false;
    const QPointer<QWidget> owner(parent);
    DiscordWebhook::get()->send(GlobalAppSettings::get()->getDiscordWebhookUrl(),QString(),{},
        [owner](bool ok,const QString &message){reportDiscordResult(owner,ok,message);},QJsonArray{jumpEmbed(jump)});
    return true;
}
}
