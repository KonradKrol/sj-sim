#include "DataPack.h"
#include <QJsonArray>

QJsonObject DataPack::toJson() const
{
    QJsonObject o{{"format", "sj-sim-pack"}, {"version", 1}, {"id", id}, {"name", name},
                  {"kind", kind == Jumpers ? "jumpers" : kind == Hills ? "hills" : "rules"}};
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
    if (o.value("format").toString() != "sj-sim-pack" || o.value("version").toInt() != 1)
        return fail(QObject::tr("Nieobsługiwany format paczki."));
    DataPack p;
    p.name = o.value("name").toString().trimmed();
    if (p.name.isEmpty()) return fail(QObject::tr("Paczka musi mieć nazwę."));
    if (!o.value("id").toString().isEmpty()) p.id = o.value("id").toString();
    const QString type = o.value("kind").toString();
    if (type == "jumpers") p.kind = Jumpers;
    else if (type == "hills") p.kind = Hills;
    else if (type == "rules") p.kind = Rules;
    else return fail(QObject::tr("Nieznany rodzaj paczki."));
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
        } else if (p.kind == Hills) p.hills.append(Hill::getFromJson(entry));
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
