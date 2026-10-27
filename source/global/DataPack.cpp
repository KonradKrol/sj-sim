#include "DataPack.h"
#include <QJsonArray>
#include <QSet>

QJsonObject DataPack::toJson() const
{
    QJsonObject o{{"format", "sj-sim-pack"}, {"version", engineVersion==2?2:1}, {"id", id}, {"name", name},
                  {"kind", kind == Jumpers ? "jumpers" : kind == Hills ? "hills" : "rules"}};
    if(!approximateMigrationSource.isEmpty())o.insert("approximate-migration-source",approximateMigrationSource);
    QJsonArray entries;
    if (kind == Jumpers) {
        o.insert("age-format", ageMode ? "age" : "birth-date");
        o.insert("reference-year", referenceYear);
        for (const auto &j : jumpers) {
            auto entry = Jumper::getJsonObject(j);
            if (ageMode && j.ageInYear(referenceYear) >= 0) entry.insert("age", j.ageInYear(referenceYear));
            entries.append(entry);
        }
        o.insert("jumpers", entries);
    } else if (kind == Hills) {
        for (const auto &h : hills) entries.append(Hill::getJsonObject(h));
        o.insert("hills", entries);
    } else {
        for (const auto &r : rules) entries.append(CompetitionRules::getJsonObject(r));
        o.insert("competitionsRules", entries);
        QJsonArray presets;
        for (auto p : points) presets.append(PointsForPlacesPreset::getJsonObject(p));
        o.insert("pointsForPlacesPresets", presets);
    }
    return o;
}

bool DataPack::fromJson(const QJsonObject &o, DataPack &result, QString &error)
{
    auto fail = [&error](const QString &s) { error = s; return false; };
    const auto format=o.value("format").toString();
    if(format=="sj-sim-athlete-pack" || format=="sj-sim-hill-pack") {
        DataPack p;p.engineVersion=2;p.kind=format=="sj-sim-athlete-pack"?Jumpers:Hills;
        p.id=o["id"].toString();p.name=o["name"].toString().trimmed();
        if(o["version"]!=1 || QUuid(p.id).isNull() || QUuid(p.id).toString(QUuid::WithoutBraces)!=p.id
            || p.name.isEmpty() || p.name.size()>100 || !o["profiles"].isArray() || o["profiles"].toArray().size()>10000)
            return fail(QObject::tr("Nieprawidłowy format nowej paczki."));
        QSet<QString> ids;
        for(const auto &value:o["profiles"].toArray()) {
            if(!value.isObject())return fail(QObject::tr("Nieprawidłowy profil w paczce."));
            QString entryId;
            if(p.kind==Jumpers) {
                AthleteProfile profile;Jumper jumper;
                if(!AthleteProfile::fromJson(value.toObject(),profile,error) || !jumper.setSimulationProfile(profile,error))return false;
                p.jumpers.append(jumper);entryId=profile.id;
            } else {
                HillProfile profile;Hill hill;
                if(!HillProfile::fromJson(value.toObject(),profile,error) || !hill.setSimulationProfile(profile,error))return false;
                p.hills.append(hill);entryId=profile.id;
            }
            if(ids.contains(entryId))return fail(QObject::tr("Powtórzony identyfikator profilu w paczce."));
            ids.insert(entryId);
        }
        error.clear();result=p;return true;
    }
    if (format != "sj-sim-pack" || (o.value("version")!=1 && o.value("version")!=2))
        return fail(QObject::tr("Nieobsługiwany format paczki."));
    DataPack p;
    p.engineVersion=o.value("version").toInt()==2?2:1;
    p.approximateMigrationSource=o.value("approximate-migration-source").toString();
    p.name = o.value("name").toString().trimmed();
    if (p.name.isEmpty()) return fail(QObject::tr("Paczka musi mieć nazwę."));
    if (!o.value("id").toString().isEmpty()) p.id = o.value("id").toString();
    const QString type = o.value("kind").toString();
    if (type == "jumpers") p.kind = Jumpers;
    else if (type == "hills") p.kind = Hills;
    else if (type == "rules") p.kind = Rules;
    else return fail(QObject::tr("Nieznany rodzaj paczki."));
    if(p.kind==Rules && p.engineVersion!=1)return fail(QObject::tr("Nieobsługiwana wersja paczki zasad."));
    const QString key = p.kind == Jumpers ? "jumpers" : p.kind == Hills ? "hills" : "competitionsRules";
    if (!o.value(key).isArray()) return fail(QObject::tr("Brak listy zawartości paczki."));
    if (p.kind == Jumpers) {
        const auto format = o.value("age-format").toString();
        if (format != "age" && format != "birth-date") return fail(QObject::tr("Nieznany format wieku."));
        p.ageMode = format == "age";
        p.referenceYear = o.value("reference-year").toInt(p.ageMode ? 0 : QDate::currentDate().year());
        if (p.referenceYear < 1 || p.referenceYear > 9999) return fail(QObject::tr("Podaj rok odniesienia od 1 do 9999."));
    }
    for (const auto &value : o.value(key).toArray()) {
        if (!value.isObject()) return fail(QObject::tr("Nieprawidłowy wpis w paczce."));
        auto entry = value.toObject();
        if (entry.value("id").toString().isEmpty())
            entry.insert("id", QUuid::createUuid().toString(QUuid::WithoutBraces));
        const auto entityId = entry.value("id").toString();
        // Legacy databases also contain the all-zero UUID; its syntax is valid.
        if (QUuid(entityId).toString(QUuid::WithoutBraces).compare(entityId, Qt::CaseInsensitive) != 0)
            return fail(QObject::tr("Nieprawidłowy identyfikator wpisu w paczce."));
        if (p.kind == Jumpers) {
            if(!Jumper::validateSimulationProfileJson(entry,error))return false;
            if(p.engineVersion==2 && !entry.contains("simulation-v2"))return fail(QObject::tr("Brak nowego profilu skoczka w paczce v2."));
            if (entry.contains("birth-year")) {
                const double year = entry.value("birth-year").toDouble(-1);
                if (year < 1 || year > 9999 || year != int(year))
                    return fail(QObject::tr("Nieprawidłowy rok urodzenia."));
                const auto date = QDate::fromString(entry.value("birth-date").toString(), Qt::ISODate);
                if (date.isValid() && date.year() != int(year))
                    return fail(QObject::tr("Data i rok urodzenia są sprzeczne."));
            }
            auto j = Jumper::getFromJson(entry);
            if (entry.contains("birth-date") && !QDate::fromString(entry.value("birth-date").toString(), Qt::ISODate).isValid())
                return fail(QObject::tr("Nieprawidłowa data urodzenia; użyj RRRR-MM-DD."));
            if (p.ageMode && entry.contains("age")) {
                const double age = entry.value("age").toDouble(-1);
                if (age < 0 || age > p.referenceYear - 1 || age != int(age)) return fail(QObject::tr("Nieprawidłowy wiek skoczka."));
                const int year = p.referenceYear - int(age);
                if (j.getBirthYear() && j.getBirthYear() != year) return fail(QObject::tr("Wiek i rok urodzenia są sprzeczne."));
                j.setBirthYear(year);
            }
            p.jumpers.append(j);
        } else if (p.kind == Hills) {
            if(!Hill::validateSimulationProfileJson(entry,error))return false;
            if(p.engineVersion==2 && !entry.contains("simulation-v2"))return fail(QObject::tr("Brak nowego profilu skoczni w paczce v2."));
            p.hills.append(Hill::getFromJson(entry));
        }
        else p.rules.append(CompetitionRules::getFromJson(entry));
    }
    if (p.kind == Rules) {
        if (!o.value("pointsForPlacesPresets").isArray()) return fail(QObject::tr("Brak listy presetów punktów."));
        for (const auto &v : o.value("pointsForPlacesPresets").toArray()) {
            if (!v.isObject()) return fail(QObject::tr("Nieprawidłowy preset punktów."));
            p.points.append(PointsForPlacesPreset::getFromJson(v.toObject()));
        }
    }
    error.clear();
    result = p;
    return true;
}
