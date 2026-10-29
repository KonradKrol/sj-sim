#include "AthleteProfile.h"
#include <QJsonArray>
#include <QObject>
#include <QUuid>
#include <QRegularExpression>
#include <cmath>

AthleteProfile AthleteProfile::create(){AthleteProfile p;p.id=QUuid::createUuid().toString(QUuid::WithoutBraces);return p;}
QJsonObject AthleteProfile::toJson() const {
    QJsonArray values;for(int p:perks)values.append(p);
    return {{"perkLevels",4},{"id",id},{"name",name},{"surname",surname},{"country",country},{"portrait",portrait},{"photoPath",photoPath},
        {"overall",overall},{"form",form},{"landing",landing},{"birthYear",birthYear},
        {"personalBest",personalBest},{"perks",values},{"sourceId",sourceId}};
}
bool AthleteProfile::fromJson(const QJsonObject &j,AthleteProfile &result,QString &error){
    const auto fail=[&](const QString &message){error=message;return false;};
    if(j["perkLevels"].toDouble()!=4)return fail(QObject::tr("Paczka wymaga jednorazowej konwersji do czterech poziomów perków."));
    AthleteProfile p;
    p.id=j["id"].toString();
    if(QUuid(p.id).isNull() || QUuid(p.id).toString(QUuid::WithoutBraces)!=p.id)return fail(QObject::tr("Nieprawidłowy identyfikator skoczka."));
    for(const auto *key:{"name","surname","country","portrait","sourceId"})if(!j[key].isString())return fail(QObject::tr("Nieprawidłowe dane profilu."));
    p.name=j["name"].toString();p.surname=j["surname"].toString();p.country=j["country"].toString();p.portrait=j["portrait"].toString();p.sourceId=j["sourceId"].toString();
    if(j.contains("photoPath") && !j["photoPath"].isString())return fail(QObject::tr("Nieprawidłowa ścieżka zdjęcia."));
    p.photoPath=j["photoPath"].toString();
    p.photoPath.replace('\\','/');
    const auto segments=p.photoPath.split('/');
    if(p.photoPath.size()>4096 || p.photoPath.contains(QChar::Null) || p.photoPath.startsWith('/') || p.photoPath.contains(':') || segments.contains("..") || segments.contains("."))
        return fail(QObject::tr("Podaj ścieżkę względną wobec folderu paczki, np. zdjecia/skoczek.jpg."));
    if(p.name.size()>100 || p.surname.size()>100 || !QRegularExpression("^[A-Z]{3}$").match(p.country).hasMatch())return fail(QObject::tr("Sprawdź imię, nazwisko i trzyliterowy kod kraju."));
    if(!p.portrait.isEmpty() && !QRegularExpression("^portraits/[a-f0-9]{64}\\.png$").match(p.portrait).hasMatch())return fail(QObject::tr("Nieprawidłowa ścieżka zdjęcia w paczce."));
    const auto integer=[&](const char *key,int lo,int hi,int &value){const double n=j[key].toDouble(-1);if(!j[key].isDouble() || !std::isfinite(n) || n!=std::floor(n) || n<lo || n>hi)return false;value=int(n);return true;};
    if(!integer("overall",1,100,p.overall) || !integer("form",1,100,p.form) || !integer("landing",0,4,p.landing) || !integer("birthYear",0,9999,p.birthYear))return fail(QObject::tr("Parametry skoczka są poza zakresem."));
    p.personalBest=j["personalBest"].toDouble(-1);if(!j["personalBest"].isDouble() || !std::isfinite(p.personalBest) || p.personalBest<0 || p.personalBest>1000)return fail(QObject::tr("Rekord musi mieścić się między 0 a 1000 m."));
    const auto perks=j["perks"].toArray();if(perks.size()!=3)return fail(QObject::tr("Nieprawidłowy zestaw perków."));
    for(int i=0;i<3;++i){const double v=perks[i].toDouble(99);if(v!=std::floor(v) || v< -4 || v>4)return fail(QObject::tr("Nieprawidłowy poziom perka."));p.perks[i]=int(v);}
    result=p;error.clear();return true;
}
