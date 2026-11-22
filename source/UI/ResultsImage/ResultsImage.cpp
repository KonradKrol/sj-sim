#include "ResultsImage.h"

#include "../../competitions/CompetitionInfo.h"
#include "../../competitions/CompetitionResults.h"
#include "../../competitions/CompetitionSingleResult.h"
#include "../../seasons/Classification.h"
#include "../../seasons/ClassificationSingleResult.h"
#include "../../simulator/Hill.h"
#include "../../simulator/Jumper.h"
#include "../../simulator/Team.h"
#include "../../global/CountryCodes.h"
#include "../../global/GlobalDatabase.h"
#include "../../global/UserDataPaths.h"
#include "../mainwindow.h"

#include <QClipboard>
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
    QImageReader reader(QStringLiteral("flags/%1.svg").arg(CountryCodes::toIso(country)));
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

int askLimit(QWidget *parent,int available,bool *accepted)
{
    QStringList options;QVector<int> limits;
    for(int limit:{10,30})if(available>limit){options<<QObject::tr("%1 pierwszych miejsc").arg(limit);limits<<limit;}
    options<<QObject::tr("Wszystkie miejsca (%1)").arg(available);limits<<0;
    const int preferred=limits.contains(30)?limits.indexOf(30):options.size()-1;
    const QString choice=QInputDialog::getItem(parent,QObject::tr("Obraz wyników"),
        QObject::tr("Ile miejsc pokazać na obrazie? Krótsza lista jest czytelniejsza w czacie."),options,preferred,false,accepted);
    if(!*accepted)return 0;
    return limits.value(options.indexOf(choice),0);
}

bool saveAndCopy(QWidget *parent,const ResultsCard &card,const QString &suggestedName)
{
    const QString folder=UserDataPaths::path(QStringLiteral("Exports"));
    QDir().mkpath(folder);
    QString path=QFileDialog::getSaveFileName(parent,QObject::tr("Zapisz obraz wyników"),
        QDir(folder).filePath(safeFileName(suggestedName)+QStringLiteral(".png")),QObject::tr("Obraz PNG (*.png)"));
    if(path.isEmpty())return false;
    if(QFileInfo(path).suffix().compare(QStringLiteral("png"),Qt::CaseInsensitive)!=0)path+=QStringLiteral(".png");
    const QImage image=ResultsImage::render(card);
    if(image.isNull() || !image.save(path,"PNG")){
        QMessageBox::warning(parent,QObject::tr("Nie zapisano obrazu"),QObject::tr("Nie udało się zapisać pliku %1. Wybierz inny folder.").arg(QDir::toNativeSeparators(path)));
        return false;
    }
    if(auto *clipboard=QGuiApplication::clipboard())clipboard->setImage(image);
    QMessageBox::information(parent,QObject::tr("Zapisano obraz wyników"),
        QObject::tr("Obraz zapisano w pliku:\n%1\n\nJest też w schowku, więc możesz go od razu wkleić, na przykład na Discordzie.").arg(QDir::toNativeSeparators(path)));
    return true;
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
        painter.drawText(QRectF(margin,120,width-2*margin,30),Qt::AlignLeft|Qt::AlignVCenter,card.subtitle);
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

bool exportCompetition(QWidget *parent,CompetitionInfo *competition)
{
    if(!competition)return false;
    const int available=competition->getResultsReference().getResultsReference().size();
    if(available==0){QMessageBox::information(parent,QObject::tr("Obraz wyników"),QObject::tr("Ten konkurs nie ma jeszcze wyników."));return false;}
    bool accepted=false;const int limit=askLimit(parent,available,&accepted);if(!accepted)return false;
    const ResultsCard card=fromCompetition(competition,limit);
    QString name=card.title+QStringLiteral(" ")+competition->getLongSerieTypeText();
    if(competition->getScheduledDate().isValid())name+=QStringLiteral(" ")+competition->getScheduledDate().toString(Qt::ISODate);
    return saveAndCopy(parent,card,name);
}

bool exportClassification(QWidget *parent,Classification *classification)
{
    if(!classification)return false;
    const int available=classification->getResultsReference().size();
    if(available==0){QMessageBox::information(parent,QObject::tr("Obraz wyników"),QObject::tr("Ta klasyfikacja nie ma jeszcze wyników."));return false;}
    bool accepted=false;const int limit=askLimit(parent,available,&accepted);if(!accepted)return false;
    return saveAndCopy(parent,fromClassification(classification,limit),classification->getName());
}
}
