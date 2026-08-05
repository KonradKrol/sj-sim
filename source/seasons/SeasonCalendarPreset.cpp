#include "SeasonCalendarPreset.h"
#include <QJsonValue>
#include <QHash>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

SeasonCalendarPreset::SeasonCalendarPreset(SeasonCalendar *seasonCalendar)
{
    if(seasonCalendar != nullptr)
    {
        SeasonCalendar * presetCalendar = &this->calendar;
        QHash<CompetitionInfo *, CompetitionInfo *> competitions; //1. Konkurs z kalendarza, 2. Konkurs z presetu
        for(auto & comp : seasonCalendar->getCompetitionsReference())
        {
            CompetitionInfo * presetCompetition = new CompetitionInfo(*comp);
            presetCompetition->reassign();
            presetCompetition->getResultsReference().reassign();
            presetCalendar->getCompetitionsReference().push_back(presetCompetition);
            competitions.insert(comp, presetCompetition);
        }
        for(auto & presetCompetition : presetCalendar->getCompetitionsReference())
        {
            for(auto & calendarCompetition : seasonCalendar->getCompetitionsReference())
            {
                if(presetCompetition->getAdvancementCompetition() == calendarCompetition)
                    presetCompetition->setAdvancementCompetition(competitions.value(calendarCompetition));
                if(presetCompetition->getTrialRound() == calendarCompetition)
                    presetCompetition->setTrialRound(competitions.value(calendarCompetition));
                for(auto & presetTraining : presetCompetition->getTrainingsReference())
                {
                    if(presetTraining == calendarCompetition){
                        presetTraining = competitions.value(calendarCompetition);
                    }
                }
            }
        }
        QHash<Classification *, Classification *> classifications; //1. Klasyfikacja z kalendarza, 2. Klasyfikacja z presetu
        for(auto & cls : seasonCalendar->getClassificationsReference())
        {
            Classification * presetClassification = new Classification(*cls);
            presetClassification->reassign();
            presetCalendar->getClassificationsReference().push_back(presetClassification);
            classifications.insert(cls, presetClassification);
        }
        for(auto & presetCompetition : presetCalendar->getCompetitionsReference())
        {
            for(auto & calendarClassification : seasonCalendar->getClassificationsReference())
            {
                if(presetCompetition->getAdvancementClassification() == calendarClassification)
                    presetCompetition->setAdvancementClassification(classifications.value(calendarClassification));
                for(auto & presetCls : presetCompetition->getClassificationsReference())
                {
                    if(presetCls == calendarClassification){
                        presetCls = classifications.value(calendarClassification);
                    }
                }
            }
        }
        presetCalendar->updateCompetitionsQualifyingCompetitions();
    }
}

SeasonCalendarPreset::~SeasonCalendarPreset()
{
    /*for(auto & comp : this->calendar.getCompetitionsReference())
        delete comp;
    for(auto & cls : this->calendar.getClassificationsReference())
        delete cls;*/
}

QJsonObject SeasonCalendarPreset::getJsonObject(SeasonCalendarPreset preset)
{
    QJsonObject object;
    object.insert("name", preset.getName());
    for(auto & comp : preset.getCalendarReference().getCompetitionsReference())
        comp->setHill(nullptr);
    object.insert("calendar", SeasonCalendar::getJsonObject(preset.getCalendarReference()));
    QJsonArray hillsArray;
    for(auto & hill : preset.getHillsReference()){
        QJsonObject hillObject;
        hillObject.insert("name", hill.first);
        hillObject.insert("hs-point", hill.second);
        hillsArray.push_back(hillObject);
    }
    object.insert("hills", hillsArray);
    return object;
}

SeasonCalendarPreset SeasonCalendarPreset::getFromJson(QJsonObject json, IdentifiableObjectsStorage * storage, QMap<ulong, Identifiable *> *before120Map)
{
    SeasonCalendarPreset preset;
    preset.setName(json.value("name").toString());
    preset.setCalendar(SeasonCalendar::getFromJson(json.value("calendar").toObject(), storage, before120Map));
    QJsonArray hillsArray = json.value("hills").toArray();
    for(auto val : hillsArray)
    {
        QPair<QString, double> hill;
        hill.first = val.toObject().value("name").toString();
        hill.second = val.toObject().value("hs-point").toDouble();
        preset.getHillsReference().push_back(hill);
    }
    return preset;
}

QVector<SeasonCalendarPreset> SeasonCalendarPreset::getVectorFromJson(QByteArray json)
{
    QVector<SeasonCalendarPreset> presets;
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(json, &error);
    if(error.error != QJsonParseError::NoError)
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Błąd przy wczytytywaniu presetów kalendarzy", "Treść błędu: " + error.errorString(),  QMessageBox::StandardButton::Ok);
        message.setModal(true);
        message.exec();
        return presets;
    }
    QJsonObject object = document.object();
    QJsonValue value = object.value("calendarPresets");
    if(!value.isArray())
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Błąd przy wczytytywaniu presetów kalendarzy","Prawdopodobnie w tym pliku nie ma listy z presetami\nUpewnij się, że wybrałeś właściwy plik!");
        message.setModal(true);
        message.exec();
        return presets;
    }
    QJsonArray array = value.toArray();
    for(const auto & val : array)
    {
        IdentifiableObjectsStorage * manager = new IdentifiableObjectsStorage();
        presets.push_back(SeasonCalendarPreset::getFromJson(val.toObject(), manager));
        delete manager;
    }
    return presets;
}

bool SeasonCalendarPreset::saveToFile(QString dir)
{
    QJsonDocument document;
    QJsonObject mainObject;
    mainObject.insert("calendar-preset", SeasonCalendarPreset::getJsonObject(*this));
    document.setObject(mainObject);
    QFile file(dir + getName() + ".json");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Nie można otworzyć pliku z presetem kalendarza " + getName(), "Nie udało się otworzyć pliku " + dir + getName() + ".json" + "\nUpewnij się, że istnieje tam taki plik lub ma on odpowiednie uprawnienia",  QMessageBox::StandardButton::Ok);
        message.setModal(true);
        message.exec();
        return false;
    }
    file.resize(0);
    file.write(document.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

QString SeasonCalendarPreset::getName() const
{
    return name;
}

void SeasonCalendarPreset::setName(const QString &newName)
{
    name = newName;
}

SeasonCalendar &SeasonCalendarPreset::getCalendarReference()
{
    return calendar;
}

void SeasonCalendarPreset::setHills(const QVector<QPair<QString, double> > &newHills)
{
    hills = newHills;
}

void SeasonCalendarPreset::setCalendar(const SeasonCalendar &newCalendar)
{
    calendar = newCalendar;
}

QVector<QPair<QString, double> > &SeasonCalendarPreset::getHillsReference()
{
    return hills;
}
