#include <QObject>
#include "SimulationSave.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QRegularExpression>
#include <QSaveFile>
#include <QSet>
#include <QSysInfo>
#include "../global/Uuid.h"
#include "../simulation-v2/SimulationCatalog.h"

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
    if(!CompetitionRules::validateWindMeasurementJson(object))return false;
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
    if(object.contains("engine-version") && (!object["engine-version"].isDouble()
        || (object["engine-version"].toDouble()!=1 && object["engine-version"].toDouble()!=2)))return false;
    const bool v2=object["engine-version"].toInt(1)==2;
    QString profileError;
    for(const auto &value:object["jumpers"].toArray())if((v2 && !value.toObject().contains("simulation-v2")) || !Jumper::validateSimulationProfileJson(value.toObject(),profileError))return false;
    for(const auto &value:object["hills"].toArray())if((v2 && !value.toObject().contains("simulation-v2")) || !Hill::validateSimulationProfileJson(value.toObject(),profileError))return false;
    return object.value(QStringLiteral("id")).isString()
        && object.value(QStringLiteral("actual-season-id")).isString()
        && object.value(QStringLiteral("jumpers")).isArray()
        && object.value(QStringLiteral("hills")).isArray()
        && object.value(QStringLiteral("rules")).isArray()
        && object.value(QStringLiteral("seasons")).isArray()
        && hasValidUuidFields(object);
}

// Saving must never turn an already-invalid in-memory graph into a file that
// will crash on its next load.  This is linear in the save size and is much
// cheaper than JSON serialization, while keeping the QSaveFile write atomic.
bool hasSerializableGraph(SimulationSave &save)
{
    if(save.getActualSeason() == nullptr)
        return false;

    for(auto *jumper : save.getJumpersReference())
        if(jumper == nullptr || (save.getEngineVersion()==2 && !jumper->getSimulationProfile()))
            return false;
    for(auto *hill : save.getHillsReference())
        if(hill == nullptr || (save.getEngineVersion()==2 && !hill->getSimulationProfile()))
            return false;
    for(const auto &list : save.getJumpersListsReference())
        for(auto *jumper : list.getJumpers())
            if(jumper == nullptr)
                return false;
    for(auto *jumper : save.getJumpersFormInstabilitiesReference().keys())
        if(jumper == nullptr)
            return false;

    for(auto &season : save.getSeasonsReference())
        for(auto *calendar : season.getCalendarsReference()) {
            if(calendar == nullptr)
                return false;
            for(auto *classification : calendar->getClassificationsReference())
                if(classification == nullptr)
                    return false;
            for(auto *competition : calendar->getCompetitionsReference()) {
                if(competition == nullptr || competition->getHill() == nullptr)
                    return false;
                for(auto *training : competition->getTrainingsReference())
                    if(training == nullptr)
                        return false;
                for(auto *classification : competition->getClassificationsReference())
                    if(classification == nullptr)
                        return false;
                for(auto *jumper : competition->getStartListReference())
                    if(jumper == nullptr)
                        return false;
            }
        }
    return true;
}
}

SimulationSave::SimulationSave() :
    Identifiable(),
    actualSeason(nullptr),
    nextCompetition(nullptr),
    nextCompetitionIndex(0),
    showForm(true),
    showInstability(true),
    saveFileSizeReduce(true)
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
    save->setEngineVersion(obj["engine-version"].toInt(1));
    if(storage == nullptr)
    {
        storage = save;
    }
    save->setID(sole::rebuild(obj.value("id").toString().toStdString()));

    qDebug()<<"id: "<<save->getID();

    QJsonArray array = obj.value("jumpers").toArray();
    QVector<Jumper *> jumpers;
    for(const auto &value : qAsConst(array)) {
        auto *jumper=new Jumper(Jumper::getFromJson(value.toObject()));
        if(jumper->getSimulationProfile() && !jumper->getSimulationPackId().isEmpty())
            jumper->setSimulationPackContext(jumper->getSimulationPackId(),QDir(SimulationV2::CatalogPaths::defaults().athletes).filePath(jumper->getSimulationPackId()));
        jumpers.append(jumper);
    }
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
    QSet<QString> listIds;
    for(const auto &value : qAsConst(array)) {
        auto list=SaveJumpersList::getFromJson(value.toObject(), storage);
        while(listIds.contains(list.getId()))list.reassignId();
        listIds.insert(list.getId());lists.append(list);
    }
    save->setJumpersLists(lists);
    if(activeRosterFeatureEnabled() && obj.contains("active-jumpers-list-id")) {
        if(!save->setActiveJumpersListId(obj.value("active-jumpers-list-id").toString()))save->setActiveJumpersListId(QString());
    } else if(activeRosterFeatureEnabled()) {
        // Preserve the previous include/exclude result when upgrading an older save.
        auto previous=save->getJumpersReference();
        for(const auto &list:lists)for(auto *jumper:list.getJumpers()) {
            if(!list.getIsDefault())previous.removeAll(jumper);
            else if(jumper && !previous.contains(jumper))previous.append(jumper);
        }
        QString selected;
        if(previous!=save->getJumpersReference()) {
            for(const auto &list:lists)if(list.getJumpers()==previous){selected=list.getId();break;}
            if(selected.isEmpty()) {
                SaveJumpersList migrated(QCoreApplication::translate("SimulationSave","Dotychczasowy skład"));
                migrated.setJumpers(previous);selected=migrated.getId();save->getJumpersListsReference().append(migrated);
            }
        }
        save->setActiveJumpersListId(selected);
    }

    save->setActualSeason(static_cast<Season *>(storage->get(obj.value("actual-season-id").toString())));
    if(save->getActualSeason() == nullptr)
    {
        delete save;
        return nullptr;
    }

    save->updateNextCompetitionIndex();

    save->setAutosaveEnabled(obj.value("autosave-enabled").toBool(true));
    save->setShowForm(obj.value("show-form").toBool(true));
    save->setShowInstability(obj.value("show-instability").toBool(true));
    save->setSaveFileSizeReduce(obj.value("save-file-size-reduce").toBool(true));

    return save;
}

QJsonObject SimulationSave::getJsonObject(SimulationSave &save)
{
    QJsonObject object;
    object.insert("engine-version",save.getEngineVersion());
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
    if(activeRosterFeatureEnabled()) {
        const auto activeList=save.activeJumpersListId();
        object.insert("active-jumpers-list-id",activeList.isEmpty()?QStringLiteral("-1"):activeList);
    }

    object.insert("actual-season-id", save.getActualSeason()->getIDStr());
    object.insert("next-competition-index", save.getNextCompetitionIndex());

    object.insert("autosave-enabled", save.getAutosaveEnabled());
    object.insert("show-form", save.getShowForm());
    object.insert("show-instability", save.getShowInstability());
    object.insert("save-file-size-reduce", save.getSaveFileSizeReduce());

    return object;
}

bool SimulationSave::saveToFile(QString dir, QString fileName)
{
    if(!hasSerializableGraph(*this))
    {
        QMessageBox::critical(nullptr, QObject::tr("Nie moĹĽna zapisaÄ‡ zapisu symulacji"),
                              QObject::tr("Zapis zawiera nieprawidĹ‚owe powiÄ…zania. Plik nie zostaĹ‚ zmieniony."));
        return false;
    }
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
    if(!targetDirectory.mkpath("."))
    {
        QMessageBox::critical(nullptr, QObject::tr("Nie można utworzyć folderu zapisu"),
                              QObject::tr("Nie udało się utworzyć folderu %1.").arg(targetDirectory.absolutePath()));
        return false;
    }
    const QString filePath = targetDirectory.absoluteFilePath(fileName + ".json");

    QJsonDocument document;
    QJsonObject mainObject;
    mainObject.insert("simulation-save", SimulationSave::getJsonObject(*this));
    document.setObject(mainObject);

    QSaveFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox message(QMessageBox::Icon::Critical, QObject::tr("Nie można otworzyć pliku z zapisem symulacji ") + getName(), QObject::tr("Nie udało się otworzyć pliku ") + filePath + QObject::tr("\nUpewnij się, że folder istnieje i ma odpowiednie uprawnienia"),  QMessageBox::StandardButton::Ok);
        message.setModal(true);
        message.exec();
        return false;
    }
    QByteArray content = document.toJson(saveFileSizeReduce ? QJsonDocument::Compact : QJsonDocument::Indented);
    if(!saveFileSizeReduce) {
        // JSON strings escape newlines/tabs: only indentation before each line is replaced.
        QByteArray readable;readable.reserve(content.size());
        for(int offset=0;offset<content.size();) {
            const int start=offset;
            while(offset<content.size() && content[offset]==' ')++offset;
            readable.append(QByteArray((offset-start)/4,'\t'));
            readable.append(QByteArray((offset-start)%4,' '));
            const int end=content.indexOf('\n',offset);
            const int length=(end<0?content.size():end+1)-offset;
            readable.append(content.constData()+offset,length);offset+=length;
        }
        content=std::move(readable);
    }
    if(file.write(content) != content.size() || !file.commit())
    {
        QMessageBox::critical(nullptr, QObject::tr("Nie można zapisać zapisu symulacji"),
                              QObject::tr("Nie udało się bezpiecznie zapisać pliku %1. Poprzedni plik pozostał bez zmian.").arg(filePath));
        return false;
    }
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
        objects.push_back(&season);
        for(auto & cal : season.getCalendarsReference())
        {
            objects.push_back(cal);
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

    QSet<QString> assignedIds;
    for(auto & object : objects)
    {
        while(object->getIDStr() == QStringLiteral("00000000-0000-0000-0000-000000000000")
              || assignedIds.contains(object->getIDStr()))
        {
            object->reassign();
        }
        assignedIds.insert(object->getIDStr());
    }
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

const SaveJumpersList *SimulationSave::activeJumpersList() const
{
    for(const auto &list:jumpersLists)if(list.getId()==activeListId)return &list;
    return nullptr;
}

QString SimulationSave::activeJumpersListId() const
{
    const auto *list=activeJumpersList();return list?list->getId():QString();
}

QVector<Jumper *> SimulationSave::activeJumpers() const
{
    const auto *list=activeJumpersList();return list?list->getJumpers():jumpers;
}

bool SimulationSave::setActiveJumpersListId(const QString &id)
{
    const QString selected=id==QStringLiteral("-1")?QString():id;
    if(!selected.isEmpty()){
        bool found=false;for(const auto &list:jumpersLists)if(list.getId()==selected){found=true;break;}
        if(!found)return false;
    }
    activeListId=selected;
    for(auto &list:jumpersLists)list.setIsDefault(!selected.isEmpty() && list.getId()==selected);
    return true;
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
