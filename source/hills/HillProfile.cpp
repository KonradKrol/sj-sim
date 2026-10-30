#include "HillProfile.h"
#include <QObject>
#include <QJsonArray>
#include <QUuid>
#include <QRegularExpression>
#include <cmath>

HillProfile HillProfile::create(){HillProfile p;p.id=QUuid::createUuid().toString(QUuid::WithoutBraces);return p;}
double HillProfile::automaticTailwindPoints(double headwindPoints){return qMin(100.,headwindPoints*1.5);}
QJsonObject HillProfile::toJson()const {
    QJsonArray values;for(int value:perks)values.append(value);
    return {{"perkLevels",4},{"id",id},{"name",name},{"country",country},{"sourceId",sourceId},{"k",k},{"hs",hs},{"record",record},
        {"pointsAtK",pointsAtK},{"pointsPerMetre",pointsPerMetre},{"gatePoints",gatePoints},{"headwindPoints",headwindPoints},{"tailwindPoints",automaticTailwindPoints(headwindPoints)},{"perks",values}};
}
bool HillProfile::fromJson(const QJsonObject &json,HillProfile &profile,QString &error){
    const auto fail=[&](const QString &message){error=message;return false;};
    if(json["perkLevels"].toDouble()!=4)return fail(QObject::tr("Paczka wymaga jednorazowej konwersji do czterech poziomów perków."));
    HillProfile p;p.id=json["id"].toString();
    if(QUuid(p.id).isNull() || QUuid(p.id).toString(QUuid::WithoutBraces)!=p.id)return fail(QObject::tr("Nieprawidłowy identyfikator skoczni."));
    for(const auto *key:{"name","country","sourceId"})if(!json[key].isString())return fail(QObject::tr("Nieprawidłowe dane skoczni."));
    p.name=json["name"].toString();p.country=json["country"].toString();p.sourceId=json["sourceId"].toString();
    if(p.name.size()>100 || !QRegularExpression("^[A-Z]{3}$").match(p.country).hasMatch())return fail(QObject::tr("Sprawdź nazwę skoczni i trzyliterowy kod kraju."));
    const auto number=[&](const char *key,double lo,double hi,double &value){value=json[key].toDouble(-1);return json[key].isDouble() && std::isfinite(value) && value>=lo && value<=hi;};
    if(!number("k",1,400,p.k) || !number("hs",p.k,500,p.hs) || !number("record",0,1000,p.record))return fail(QObject::tr("Punkt K musi być dodatni, HS nie mniejszy od K, a rekord w zakresie 0–1000 m."));
    if(!number("pointsAtK",0,200,p.pointsAtK) || !number("pointsPerMetre",0,20,p.pointsPerMetre) || !number("gatePoints",0,100,p.gatePoints) || !number("headwindPoints",0,100,p.headwindPoints) || !number("tailwindPoints",0,100,p.tailwindPoints))return fail(QObject::tr("Punktacja skoczni jest poza zakresem."));
    p.tailwindPoints=automaticTailwindPoints(p.headwindPoints);
    if(!json["perks"].isArray())return fail(QObject::tr("Nieprawidłowy zestaw perków skoczni."));
    const auto values=json["perks"].toArray();
    if(values.size()!=3 && values.size()!=4)return fail(QObject::tr("Nieprawidłowy zestaw perków skoczni."));
    for(int i=0;i<values.size();++i){double value=values[i].toDouble(99);if(!values[i].isDouble() || !std::isfinite(value) || value!=std::floor(value) || value< -4 || value>4)return fail(QObject::tr("Nieprawidłowy poziom perka."));p.perks[std::size_t(i)]=int(value);}
    profile=p;error.clear();return true;
}
double HillProfile::realHs()const {
    static const double easy[]{0,0.0225,0.045,0.07,0.1},hard[]{0,0.01125,0.0225,0.035,0.05};
    const int value=qBound(-4,perks[1],4);return hs*(value>=0?1+easy[value]:1-hard[-value]);
}
double HillProfile::takeoffBias()const{static const double strength[]{0,0.1125,0.225,0.3625,0.55};const int value=qBound(-4,perks[0],4);return (value>=0?1:-1)*strength[std::abs(value)];}
double HillProfile::windVariability()const{static const double variable[]{1,1.2625,1.575,1.975,2.5},stable[]{1,0.8125,0.625,0.4375,0.25};const int value=qBound(-4,perks[2],4);return value>=0?variable[value]:stable[-value];}
double HillProfile::headwindProbability()const{static const double probability[]{0.5,0.65,0.77,0.865,0.94};const int value=qBound(-4,perks[3],4);return value>=0?probability[value]:1-probability[-value];}
