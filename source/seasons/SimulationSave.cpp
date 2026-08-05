#include "SimulationSave.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QRegularExpression>
#include <QSysInfo>
#include "../global/Uuid.h"

extern Uuid globalIDGenerator;

namespace
{
bool isValidUuid(const QString &value)
{
    static const QRegularExpression standardUuid(
        QStringLiteral("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$"));
    static const QRegularExpression base62Uuid(
        QStringLiteral("^[0-9A-Za-z]+-[0-9A-Za-z]+$"));
    return standardUuid.match(value).hasMatch() || base62Uuid.match(value).hasMatch();
}

bool hasValidUuidFields(const QJsonValue &value)
{
    if(value.isArray())
    {
        for(const auto &item : value.toArray())
            if(!hasValidUuidFields(item))
                return false;
        return true;
    }

    if(!value.isObject())
        return true;

    const QJsonObject object = value.toObject();
    for(auto it = object.constBegin(); it != object.constEnd(); ++it)
    {
        if(it.key() == QStringLiteral("id") || it.key().endsWith(QStringLiteral("-id")))
        {
            if(!it.value().isString())
                return false;
            const QString id = it.value().toString();
            if(id != QStringLiteral("-1") && !isValidUuid(id))
                return false;
            if(it.key() == QStringLiteral("id") && id == QStringLiteral("-1"))
                return false;
        }
        if(!hasValidUuidFields(it.value()))
            return false;
    }
    return true;
}

bool hasRequiredSaveStructure(const QJsonObject &object)
{
    return object.value(QStringLiteral("id")).isString()
        && object.value(QStringLiteral("actual-season-id")).isString()
        && object.value(QStringLiteral("jumpers")).isArray()
        && object.value(QStringLiteral("hills")).isArray()
        && object.value(QStringLiteral("rules")).isArray()
        && object.value(QStringLiteral("seasons")).isArray()
        && hasValidUuidFields(object);
}
}

SimulationSave::SimulationSave() :
    Identifiable(),
    actualSeason(nullptr),
    nextCompetition(nullptr),
    nextCompetitionIndex(0),
    showForm(true),
    showInstability(true),
    saveFileSizeReduce(false)
{}

SimulationSave::~SimulationSave()
{
    for(auto & hill : hills)
        delete hill;
    for(auto & jumper : jumpers)
        delete jumper;
    for(auto & season : seasons)
    {
        for(auto & cal : season.getCalendarsReference())
        {
            for(auto & competition : cal->getCompetitionsReference())
                delete competition;
            for(auto & classification : cal->getClassificationsReference())
                delete classification;
        }
    }
}

SimulationSave * SimulationSave::getFromJson(QJsonObject obj, IdentifiableObjectsStorage * storage)
{
    if(!hasRequiredSaveStructure(obj))
        return nullptr;

    SimulationSave * save = new SimulationSave();
    if(storage == nullptr)
    {
        storage = save;
    }
    save->setID(sole::rebuild(obj.value("id").toString().toStdString()));

    qDebug()<<"id: "<<save->getID();

    QJsonArray array = obj.value("jumpers").toArray();
    QVector<Jumper *> jumpers;
    for(const auto &value : qAsConst(array))
        jumpers.append(new Jumper(Jumper::getFromJson(value.toObject())));
    save->setJumpers(jumpers);
    storage->add(save->getJumpersReference());

    qDebug()<<"jumpers";

    array = obj.value("hills").toArray();
    QVector<Hill *> hills;
    for(const auto &value : qAsConst(array))
        hills.append(new Hill(Hill::getFromJson(value.toObject())));
    save->setHills(hills);
    storage->add(save->getHillsReference());

    array = obj.value("rules").toArray();
    QVector<CompetitionRules> rules;
    for(const auto &value : qAsConst(array))
        rules.append(CompetitionRules::getFromJson(value.toObject()));
    save->setCompetitionRules(rules);
    storage->add(save->getCompetitionRulesReference());

    qDebug()<<"rules";

    QJsonArray seasonsArray = obj.value("seasons").toArray();
    QVector<Season> seasons;
    for(const auto &value : seasonsArray)
        seasons.append(Season::getFromJson(value.toObject(), storage));
    save->setSeasons(seasons);
    storage->add(save->getSeasonsReference());
        /*Season season = Season::getFromJson(val.toObject(), objectsManager);
        save->getSeasonsReference().push_back(season);*/


    array = obj.value("jumpers-form-instabilities").toArray();
    QHash<Jumper *, double> instabilitiesHash;
    for(const auto &value : qAsConst(array)) {
        const QJsonObject instability = value.toObject();
        instabilitiesHash.insert(static_cast<Jumper *>(storage->get(instability.value("jumper-id").toString())),
                                 instability.value("instability").toString().toDouble());
    }
    save->setJumpersFormInstabilities(instabilitiesHash);
    qDebug()<<save->getJumpersFormInstabilitiesReference().count();
    if(!save->getJumpersReference().isEmpty())
        qDebug()<<save->getJumpersFormInstabilitiesReference().value(save->getJumpersReference().first());

    array = obj.value("jumpers-lists").toArray();
    QVector<SaveJumpersList> lists;
    for(const auto &value : qAsConst(array))
        lists.append(SaveJumpersList::getFromJson(value.toObject(), storage));
    save->setJumpersLists(lists);

    save->setActualSeason(static_cast<Season *>(storage->get(obj.value("actual-season-id").toString())));
    if(save->getActualSeason() == nullptr)
    {
        delete save;
        return nullptr;
    }

    const int savedCompetitionIndex = obj.value("next-competition-index").toInt(-1);
    SeasonCalendar *actualCalendar = save->getActualSeason()->getActualCalendar();
    if(actualCalendar != nullptr
        && savedCompetitionIndex >= 0
        && savedCompetitionIndex < actualCalendar->getCompetitionsReference().count())
    {
        save->setNextCompetitionIndex(savedCompetitionIndex);
        save->setNextCompetition(actualCalendar->getCompetitionsReference().at(savedCompetitionIndex));
    }
    else
    {
        save->updateNextCompetitionIndex();
    }

    save->setShowForm(obj.value("show-form").toBool(true));
    save->setShowInstability(obj.value("show-instability").toBool(true));
    save->setSaveFileSizeReduce(obj.value("save-file-size-reduce").toBool(false));

    return save;
}

QJsonObject SimulationSave::getJsonObject(SimulationSave &save)
{
    QJsonObject object;
    object.insert("id", save.getIDStr());

    QJsonArray jumpersArray;
    for(auto *jumper : save.getJumpersReference())
        jumpersArray.append(Jumper::getJsonObject(*jumper));
    object.insert("jumpers", jumpersArray);

    QJsonArray hillsArray;
    for(auto *hill : save.getHillsReference())
        hillsArray.append(Hill::getJsonObject(*hill));
    object.insert("hills", hillsArray);

    QJsonArray rulesArray;
    for(const auto &rule : save.getCompetitionRulesReference())
        rulesArray.append(CompetitionRules::getJsonObject(rule));
    object.insert("rules", rulesArray);

    QJsonArray seasonsArray;
    for(const auto &season : save.getSeasonsReference())
        seasonsArray.append(Season::getJsonObject(season));
    object.insert("seasons", seasonsArray);

    QJsonArray instabilitiesArray;
    for(const auto &instability : save.getJumpersFormInstabilitiesToVector()) {
        QJsonObject object;
        object.insert("jumper-id", instability.first->getIDStr());
        object.insert("instability", QString::number(instability.second));
        instabilitiesArray.append(object);
    }
    object.insert("jumpers-form-instabilities", instabilitiesArray);

    QJsonArray listsArray;
    for(const auto &list : save.getJumpersListsReference())
        listsArray.append(SaveJumpersList::getJsonObject(list));
    object.insert("jumpers-lists", listsArray);

    object.insert("actual-season-id", save.getActualSeason()->getIDStr());
    object.insert("next-competition-index", save.getNextCompetitionIndex());

    object.insert("show-form", save.getShowForm());
    object.insert("show-instability", save.getShowInstability());
    object.insert("save-file-size-reduce", save.getSaveFileSizeReduce());

    return object;
}

bool SimulationSave::saveToFile(QString dir, QString fileName)
{
    if(fileName == "!default")
        fileName = getName();

    QDir targetDirectory(dir);
    if(QDir::isRelativePath(dir))
    {
        QDir applicationDirectory(QCoreApplication::applicationDirPath());
        if(!applicationDirectory.exists("userData") && QSysInfo::productType() == "windows")
            applicationDirectory.cdUp();
        targetDirectory = QDir(applicationDirectory.absoluteFilePath(dir));
    }
    targetDirectory.mkpath(".");
    const QString filePath = targetDirectory.absoluteFilePath(fileName + ".json");

    QJsonDocument document;
    QJsonObject mainObject;
    repairDatabase();
    mainObject.insert("simulation-save", SimulationSave::getJsonObject(*this));
    document.setObject(mainObject);

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Nie można otworzyć pliku z zapisem symulacji " + getName(), "Nie udało się otworzyć pliku " + filePath + "\nUpewnij się, że folder istnieje i ma odpowiednie uprawnienia",  QMessageBox::StandardButton::Ok);
        message.setModal(true);
        message.exec();
        return false;
    }
    file.resize(0);
    if(saveFileSizeReduce == true)
        file.write(document.toJson(QJsonDocument::Compact));
    else
        file.write(document.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

SimulationSave *SimulationSave::loadFromFile(QString filePath)
{
    QFile file(filePath);
    if(!file.open(QFile::ReadOnly | QFile::Text))
        return nullptr;

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();
    if(parseError.error != QJsonParseError::NoError || !document.isObject())
        return nullptr;

    const QJsonValue saveValue = document.object().value(QStringLiteral("simulation-save"));
    if(!saveValue.isObject())
        return nullptr;

    SimulationSave *save = SimulationSave::getFromJson(saveValue.toObject(), nullptr);
    if(save != nullptr)
        save->setName(QFileInfo(filePath).completeBaseName());
    return save;
}

void SimulationSave::updateNextCompetitionIndex()
{
    nextCompetitionIndex = 0;
    nextCompetition = nullptr;
    if(actualSeason == nullptr || actualSeason->getActualCalendar() == nullptr)
        return;

    for(auto & comp : actualSeason->getActualCalendar()->getCompetitionsReference())
    {
        if(comp->getPlayed() == false)
            break;
        nextCompetitionIndex++;
    }
    if(nextCompetitionIndex == actualSeason->getActualCalendar()->getCompetitionsReference().count())
        nextCompetition = nullptr;
    else
        nextCompetition = actualSeason->getActualCalendar()->getCompetitionsReference().at(nextCompetitionIndex);
}

void SimulationSave::repairDatabase()
{
    QVector<Identifiable *> objects;
    for(auto & jumper : jumpers)
        objects.push_back(jumper);
    for(auto & hill : hills)
        objects.push_back(hill);
    for(auto & rules : competitionRules)
        objects.push_back(&rules);
    for(auto & season : seasons)
    {
        for(auto & cal : season.getCalendarsReference())
        {
            for(auto & competition : cal->getCompetitionsReference())
            {
                objects.push_back(competition);
                objects.push_back(&competition->getResultsReference());
                for(auto & groups : competition->getRoundsKOGroupsReference())
                {
                    for(auto & group : groups)
                        objects.push_back(&group);
                }
                for(auto & team : competition->getTeamsReference())
                    objects.push_back(&team);
                for(auto & result : competition->getResultsReference().getResultsReference())
                    objects.push_back(&result);
            }
            for(auto & classification : cal->getClassificationsReference())
            {
                objects.push_back(classification);
                for(auto & result : classification->getResultsReference())
                {
                    objects.push_back(result);
                }
            }
        }
    }

    //globalIDGenerator.reset();
    for(auto & object : objects)
    {
        object->reassign();
    }

    return;
}

void SimulationSave::fixJumpersFormInstabilities()
{
    for(auto & j : jumpers)
    {
        if(getJumperFormInstability(j) == nullptr)
            jumpersFormInstabilities.insert(j, 0);
    }
}

double * SimulationSave::getJumperFormInstability(Jumper * j)
{
    if(jumpersFormInstabilities.contains(j) == false)
        return nullptr;
    else
        return &jumpersFormInstabilities[j];
}

bool SimulationSave::getShowInstability() const
{
    return showInstability;
}

void SimulationSave::setShowInstability(bool newShowInstability)
{
    showInstability = newShowInstability;
}

QVector<QPair<Jumper *, double> > SimulationSave::getJumpersFormInstabilitiesToVector()
{
    QVector<QPair<Jumper *, double>> vec;
    for(auto & key : jumpersFormInstabilities.keys())
    {
        QPair<Jumper *, double> p;
        p.first = key;
        p.second = jumpersFormInstabilities.value(key);
        vec.push_back(p);
    }
    return vec;
}

void SimulationSave::setJumpersFormInstabilities(const QHash<Jumper *, double> &newJumpersFormInstabilities)
{
    jumpersFormInstabilities = newJumpersFormInstabilities;
}

QVector<SaveJumpersList> SimulationSave::getJumpersLists() const
{
    return jumpersLists;
}

QVector<SaveJumpersList> &SimulationSave::getJumpersListsReference()
{
    return jumpersLists;
}

void SimulationSave::setJumpersLists(const QVector<SaveJumpersList> &newJumpersLists)
{
    jumpersLists = newJumpersLists;
}

QHash<Jumper *, double> &SimulationSave::getJumpersFormInstabilitiesReference()
{
    return jumpersFormInstabilities;
}

bool SimulationSave::getSaveFileSizeReduce() const
{
    return saveFileSizeReduce;
}

void SimulationSave::setSaveFileSizeReduce(bool newSaveFileSizeReduce)
{
    saveFileSizeReduce = newSaveFileSizeReduce;
}

bool SimulationSave::getShowForm() const
{
    return showForm;
}

void SimulationSave::setShowForm(bool newShowForm)
{
    showForm = newShowForm;
}

CompetitionInfo *SimulationSave::getNextCompetition() const
{
    return nextCompetition;
}

void SimulationSave::setNextCompetition(CompetitionInfo *newNextCompetition)
{
    nextCompetition = newNextCompetition;
}

int SimulationSave::getNextCompetitionIndex() const
{
    return nextCompetitionIndex;
}

void SimulationSave::setNextCompetitionIndex(int newNextCompetitionIndex)
{
    nextCompetitionIndex = newNextCompetitionIndex;
}

Season *SimulationSave::getActualSeason() const
{
    return actualSeason;
}

void SimulationSave::setActualSeason(Season *newActualSeason)
{
    actualSeason = newActualSeason;
}

void SimulationSave::setCompetitionRules(const QVector<CompetitionRules> &newCompetitionRules)
{
    competitionRules = newCompetitionRules;
}

QVector<Hill *> SimulationSave::getHills() const
{
    return hills;
}

QVector<CompetitionRules> &SimulationSave::getCompetitionRulesReference()
{
    return competitionRules;
}

void SimulationSave::setHills(const QVector<Hill *> &newHills)
{
    hills = newHills;
}

QVector<Jumper *> SimulationSave::getJumpers() const
{
    return jumpers;
}

QVector<Jumper *> &SimulationSave::getJumpersReference()
{
    return jumpers;
}

void SimulationSave::setJumpers(const QVector<Jumper *> &newJumpers)
{
    jumpers = newJumpers;
}

QVector<Hill *> &SimulationSave::getHillsReference()
{
    return hills;
}

QVector<Season> SimulationSave::getSeasons() const
{
    return seasons;
}

QVector<Season> &SimulationSave::getSeasonsReference()
{
    return seasons;
}

void SimulationSave::setSeasons(const QVector<Season> &newSeasons)
{
    seasons = newSeasons;
}

QString SimulationSave::getName() const
{
    return name;
}

void SimulationSave::setName(const QString &newName)
{
    name = newName;
}
