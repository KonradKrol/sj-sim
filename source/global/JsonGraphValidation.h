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
    bool collection(const QJsonObject &object,const QString &name) {
        if (object.value(name).isArray()) return true;
        error="Invalid or missing array: "+name;
        return false;
    }
    bool array(const QJsonValue &value,Kind kind,bool shared=false) {
        if (!value.isArray()) { error="Invalid or missing object collection"; return false; }
        QSet<QString> local;
        for(const auto &item:value.toArray()) {
            if (!item.isObject()) { error="Collection entry must be an object"; return false; }
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
        if(!collection(object,"competitions") || !collection(object,"classifications")) return false;
        if(!add(object,Calendar) || !array(object.value("classifications"),Classification))return false;
        for(const auto &value:object.value("classifications").toArray())
            if(!array(value.toObject().value("results"),ClassificationResult))return false;
        for(const auto &value:object.value("competitions").toArray()) {
            const auto competition=value.toObject();
            // Rules and squads are snapshots: the same identity can occur in
            // several events, but cannot be reused for another object type.
            if(!add(competition,Competition) || !add(competition.value("rules").toObject(),Rules,true)
                || !array(competition.value("teams"),Team,true))return false;
            if(!collection(competition,"rounds-ko-groups")) return false;
            for(const auto &round:competition.value("rounds-ko-groups").toArray())
                if(!array(round,Group))return false;
            const auto results=competition.value("results").toObject();
            if(!add(results,Results) || !array(results.value("results"),SingleResult))return false;
            for(const auto &result:results.value("results").toArray())
                if(!array(result.toObject().value("jumps"),Jump))return false;
        }
        return true;
    }
    bool localReferences(const QJsonValue &value,const QSet<QString> &events,const QSet<QString> &classifications) {
        if(value.isArray()){for(const auto &entry:value.toArray())if(!localReferences(entry,events,classifications))return false;return true;}
        if(!value.isObject())return true;
        const auto object=value.toObject();
        const QSet<QString> eventFields{"trial-round-id","advancement-competition-id","pending-advancement-competition-id"};
        const QSet<QString> classificationFields{"classification-id","advancement-classification-id"};
        for(auto it=object.begin();it!=object.end();++it){
            if(eventFields.contains(it.key()) || classificationFields.contains(it.key())) {
                const auto id=it.value().toString();
                if(!id.isEmpty() && id!="-1" && !(eventFields.contains(it.key())?events:classifications).contains(key(id))){error="Reference belongs to another calendar: "+it.key();return false;}
            }
            if(it.key()=="training-ids" || it.key()=="classifications-ids")for(const auto &entry:it.value().toArray())
                if(!(it.key()=="training-ids"?events:classifications).contains(key(entry.toString()))){error="List reference belongs to another calendar: "+it.key();return false;}
            if(!localReferences(it.value(),events,classifications))return false;
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
            if(targets.contains(it.key())) {
                if(!it.value().isString()){error="Reference must be a string: "+it.key();return false;}
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
    bool ok=registry.collection(object,"seasons") && registry.add(object,Registry::Save) && registry.array(object.value("jumpers"),Registry::Jumper)
        && registry.array(object.value("hills"),Registry::Hill) && registry.array(object.value("rules"),Registry::Rules);
    for(const auto &value:object.value("seasons").toArray()) {
        if(!ok)break;
        const auto season=value.toObject();ok=registry.collection(season,"calendars") && registry.add(season,Registry::Season);
        for(const auto &calendar:season.value("calendars").toArray())if(ok)ok=registry.calendar(calendar.toObject());
    }
    if(ok)ok=registry.references(object);
    if(ok)for(const auto &value:object.value("seasons").toArray()) {
        const auto season=value.toObject();QSet<QString> calendars;
        for(const auto &entry:season.value("calendars").toArray()) {
            const auto calendar=entry.toObject();calendars.insert(registry.key(calendar.value("id").toString()));
            QSet<QString> events,classifications;
            for(const auto &event:calendar.value("competitions").toArray())events.insert(registry.key(event.toObject().value("id").toString()));
            for(const auto &classification:calendar.value("classifications").toArray())classifications.insert(registry.key(classification.toObject().value("id").toString()));
            if(!registry.localReferences(calendar,events,classifications)){ok=false;break;}
        }
        if(!ok)break;
        const auto active=season.value("actual-calendar-id").toString();
        if(!active.isEmpty() && active!="-1" && !calendars.contains(registry.key(active))){registry.error="Active calendar belongs to another season";ok=false;break;}
    }
    error=registry.error;return ok;
}
}
