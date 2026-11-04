#pragma once
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSet>
#include "../3rdparty/sole.hpp"

// Validate identities before deserialization can publish untyped pointers.
namespace JsonGraphValidation {
inline bool validId(const QString &id) {
    static const QRegularExpression pattern(QStringLiteral("^(?:[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}|[0-9A-Za-z]+-[0-9A-Za-z]+)$"));
    return pattern.match(id).hasMatch();
}
inline QString canonicalId(const QString &id) {return QString::fromStdString(sole::rebuild(id.toStdString()).str());}

class Registry {
public:
    enum Kind { Save, Jumper, Hill, Rules, Season, Calendar, Classification, ClassificationResult, Competition, Results, SingleResult, Jump, Team, Group };
    QHash<QString,Kind> types;
    QString error;
    bool legacyIds=false;
    bool requireExisting=false;
    QString key(const QString &id) const {return legacyIds && !id.contains('-')?QStringLiteral("legacy:")+id:canonicalId(id);}
    bool accepts(const QString &id) const {bool numeric=false;if(legacyIds && !id.isEmpty())id.toULongLong(&numeric);return numeric || validId(id);}
    bool add(const QJsonObject &object,Kind kind,bool shared=false) {
        const auto id=object.value("id").toString();
        if(!accepts(id)){error="Invalid or missing object UUID";return false;}
        const auto identity=key(id);
        if(types.contains(identity) && (!shared || types.value(identity)!=kind)){error=QString("Duplicate object UUID: %1 (types %2, %3)").arg(id).arg(types.value(identity)).arg(kind);return false;}
        types.insert(identity,kind);return true;
    }
    bool array(const QJsonValue &value,Kind kind,bool shared=false) {
        QSet<QString> local;
        for(const auto &item:value.toArray()) {
            const auto object=item.toObject();
            const auto id=object.value("id").toString();
            if(!add(object,kind,shared))return false;
            const auto identity=key(id);
            if(local.contains(identity)){error="Duplicate UUID in collection: "+id;return false;}
            local.insert(identity);
        }
        return true;
    }
    bool calendar(const QJsonObject &object) {
        if(!add(object,Calendar) || !array(object.value("classifications"),Classification))return false;
        for(const auto &value:object.value("classifications").toArray())
            if(!array(value.toObject().value("results"),ClassificationResult))return false;
        for(const auto &value:object.value("competitions").toArray()) {
            const auto competition=value.toObject();
            // Rules and squads are snapshots: the same identity can occur in
            // several events, but cannot be reused for another object type.
            if(!add(competition,Competition) || !add(competition.value("rules").toObject(),Rules,true)
                || !array(competition.value("teams"),Team,true))return false;
            for(const auto &round:competition.value("rounds-ko-groups").toArray())
                if(!array(round,Group))return false;
            const auto results=competition.value("results").toObject();
            if(!add(results,Results) || !array(results.value("results"),SingleResult))return false;
            for(const auto &result:results.value("results").toArray())
                if(!array(result.toObject().value("jumps"),Jump))return false;
        }
        return true;
    }
    bool references(const QJsonValue &value) {
        if(value.isArray()){for(const auto &item:value.toArray())if(!references(item))return false;return true;}
        if(!value.isObject())return true;
        static const QHash<QString,Kind> targets={{"jumper-id",Jumper},{"hill-id",Hill},{"team-id",Team},{"classification-id",Classification},{"actual-season-id",Season},{"actual-calendar-id",Calendar},{"trial-round-id",Competition},{"advancement-competition-id",Competition},{"pending-advancement-competition-id",Competition},{"advancement-classification-id",Classification}};
        static const QHash<QString,Kind> lists={{"jumpers-ids",Jumper},{"start-list",Jumper},{"training-ids",Competition},{"classifications-ids",Classification}};
        const auto object=value.toObject();
        for(auto it=object.begin();it!=object.end();++it){
            if(targets.contains(it.key()) && it.value().isString()) {
                const auto id=it.value().toString();
                if(id!="-1" && !id.isEmpty()) {
                    if(!accepts(id)){error="Invalid reference UUID: "+id;return false;}
                    const auto identity=key(id);
                    if(requireExisting && !types.contains(identity)){error="Missing referenced object: "+it.key()+" = "+id;return false;}
                    if(types.contains(identity) && types.value(identity)!=targets.value(it.key())){error="Reference has the wrong object type: "+it.key();return false;}
                }
            }
            if(lists.contains(it.key())) {
                if(!it.value().isArray()){error="Invalid participant list: "+it.key();return false;}
                for(const auto &item:it.value().toArray()) {
                    const auto id=item.toString();
                    if(!item.isString() || !accepts(id)){error="Invalid participant UUID";return false;}
                    const auto identity=key(id);
                    if(requireExisting && !types.contains(identity)){error="Missing participant: "+id;return false;}
                    if(types.contains(identity) && types.value(identity)!=lists.value(it.key())){error="Participant reference has the wrong object type";return false;}
                }
            }
            if(!references(it.value()))return false;
        }
        return true;
    }
};

inline bool validateSave(const QJsonObject &object,QString &error) {
    Registry registry;
    registry.requireExisting=true;
    bool ok=registry.add(object,Registry::Save) && registry.array(object.value("jumpers"),Registry::Jumper)
        && registry.array(object.value("hills"),Registry::Hill) && registry.array(object.value("rules"),Registry::Rules);
    for(const auto &value:object.value("seasons").toArray()) {
        if(!ok)break;
        const auto season=value.toObject();ok=registry.add(season,Registry::Season);
        for(const auto &calendar:season.value("calendars").toArray())if(ok)ok=registry.calendar(calendar.toObject());
    }
    if(ok)ok=registry.references(object);
    error=registry.error;return ok;
}
}
