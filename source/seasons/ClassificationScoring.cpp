#include "ClassificationScoring.h"
#include <QJsonArray>
#include <QSet>
#include <cmath>

namespace {
QJsonArray tableJson(const QMap<int,double> &table) {
    QJsonArray a;
    for (int i=1; i<= (table.isEmpty()?0:table.lastKey()); ++i) a.append(table.value(i));
    return a;
}
QMap<int,double> readTable(const QJsonValue &v) {
    QMap<int,double> table; int i=1;
    for (auto p:v.toArray()) { if(std::isfinite(p.toDouble())) table.insert(i,p.toDouble()); ++i; }
    return table;
}
}
QJsonObject ClassificationOptions::toJson() const {
    QJsonArray variants;
    for (const auto &m:methods) variants.append(QJsonObject{{"id",m.id},{"name",m.name},{"type",m.type},
        {"points",tableJson(m.points)},{"alternative-points",tableJson(m.alternativePoints)}});
    return {{"methods",variants},{"context-parameters",contextParameters},{"age-filter",ageFilter},
        {"minimum-age",minimumAge},{"maximum-age",maximumAge},{"reference-year",referenceYear},
        {"include-unknown-age",includeUnknownAge},{"country-selection",countrySelection},{"country-count",countryCount}};
}
ClassificationOptions ClassificationOptions::fromJson(const QJsonObject &o) {
    ClassificationOptions s;
    QSet<QString> ids;
    for (auto v:o.value("methods").toArray()) {
        auto o=v.toObject(); ClassificationScoringMethod m;
        m.id=o.value("id").toString(); if(m.id.isEmpty() || ids.contains(m.id)) continue;
        ids.insert(m.id); m.name=o.value("name").toString(); m.type=qBound(0,o.value("type").toInt(),1);
        m.points=readTable(o.value("points")); m.alternativePoints=readTable(o.value("alternative-points")); s.methods.append(m);
    }
    s.contextParameters=o.value("context-parameters").toBool(); s.ageFilter=o.value("age-filter").toBool();
    s.minimumAge=qBound(0,o.value("minimum-age").toInt(),9998);
    s.maximumAge=qBound(s.minimumAge,o.value("maximum-age").toInt(150),9998);
    s.referenceYear=qBound(1,o.value("reference-year").toInt(s.referenceYear),9999);
    s.includeUnknownAge=o.value("include-unknown-age").toBool();
    s.countrySelection=qBound(0,o.value("country-selection").toInt(),2);
    s.countryCount=qBound(1,o.value("country-count").toInt(1),10000);
    return s;
}
QJsonObject ClassificationCompetitionSettings::toJson() const { return {{"method",methodId},{"parameters",parameters}}; }
ClassificationCompetitionSettings ClassificationCompetitionSettings::fromJson(const QJsonObject &o) {
    ClassificationCompetitionSettings s; s.methodId=o.value("method").toString();
    const auto p=o.value("parameters").toObject();
    for (const auto &key:{"multiplier","points-limit","reference-year","minimum-age","maximum-age","country-count"}) {
        const auto v=p.value(key);
        if(!v.isDouble() || !std::isfinite(v.toDouble()) || v.toDouble()<0) continue;
        if(QString(key)=="multiplier" && v.toDouble()>1000000) continue;
        if(QString(key)!="multiplier" && (v.toDouble()>10000 || v.toDouble()!=int(v.toDouble()))) continue;
        if((QString(key)=="reference-year" || QString(key)=="country-count") && v.toInt()<1) continue;
        if(QString(key)=="reference-year" && v.toInt()>9999) continue;
        s.parameters.insert(key,v);
    }
    return s;
}
