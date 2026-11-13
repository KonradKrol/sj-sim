#include "../simulation-v2/SimulationCatalog.h"
#include "UserDataPaths.h"
#include "AtomicJsonFile.h"
#include <QObject>
#include "GlobalDatabase.h"
#include "CountryCodes.h"
#include "../seasons/SimulationSaveListOrder.h"

#include <QFile>
#include <QSaveFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QCoreApplication>
#include <QJsonValue>
#include <QSysInfo>
#include <QJsonParseError>
#include <QJsonArray>
#include <QMessageBox>
#include <QProgressDialog>

#include "../global/CountryFlagsManager.h"

extern Uuid globalIDGenerator;

GlobalDatabase::GlobalDatabase()
{
    loadedSimulationSaves = false;
    packsWritable = false;
}

GlobalDatabase::~GlobalDatabase()
{
    for(auto & save : globalSimulationSaves)
        delete save;
    for(auto & calendarPreset : globalCalendarPresets){
        for(auto & comp : calendarPreset.getCalendarReference().getCompetitionsReference())
            delete comp;
        for(auto & cls : calendarPreset.getCalendarReference().getClassificationsReference())
            delete cls;
    }
}

void GlobalDatabase::repairDatabase()
{
    GlobalDatabase * db = this;
    /*if(db->getLoadedSimulationSaves() == false){
        db->loadSimulationSaves(true);
        db->setLoadedSimulationSaves(true);
    }*/
    QProgressDialog dialog;
    dialog.setStyleSheet("QProgressDialog{background-color: white; color: black;}");
    dialog.setMinimum(0);
    dialog.setMaximum(db->getEditableGlobalJumpers().count() +  db->getEditableGlobalHills().count()
                      + db->getEditableCompetitionRules().count() + db->getEditableGlobalSimulationSaves().count() + 1);
    dialog.setMinimumDuration(0);
    dialog.setValue(0);
    dialog.setWindowTitle(QObject::tr("Naprawa bazy danych"));
    //dialog.setLabelText(QString(QObject::tr("Postęp naprawy bazy danych: %1 z %2")).arg(QString::number(dialog.value())).arg(QString::number(dialog.maximum())));
    dialog.setModal(true);
    dialog.setWindowModality(Qt::WindowModal);

    QSet<QString> assignedIds;
    const auto repairDuplicateId = [&assignedIds](Identifiable *object){
        if(assignedIds.contains(object->getIDStr()))
            object->reassign();
        assignedIds.insert(object->getIDStr());
    };

    for(auto & jumper : db->getEditableGlobalJumpers()){
        repairDuplicateId(&jumper);
        dialog.setValue(dialog.value() + 1);
        QCoreApplication::processEvents();
    }
    for(auto & hill : db->getEditableGlobalHills()){
        repairDuplicateId(&hill);
        dialog.setValue(dialog.value() + 1);
        QCoreApplication::processEvents();
    }
    for(auto & rules : db->getEditableCompetitionRules()){
        repairDuplicateId(&rules);
        dialog.setValue(dialog.value() + 1);
        QCoreApplication::processEvents();
    }
    for(auto & calendarPreset : db->getEditableGlobalCalendarPresets())
    {
        repairDuplicateId(&calendarPreset.getCalendarReference());
        for(auto & competition : calendarPreset.getCalendarReference().getCompetitionsReference())
        {
            repairDuplicateId(competition);
            repairDuplicateId(&competition->getResultsReference());

            for(auto & groups : competition->getRoundsKOGroupsReference())
            {
                for(auto & group : groups){
                    repairDuplicateId(&group);
                }
            }
            for(auto & team : competition->getTeamsReference())
            {
                repairDuplicateId(&team);
            }
        }
        for(auto & classification : calendarPreset.getCalendarReference().getClassificationsReference())
        {
            repairDuplicateId(classification);
        }
    }
    for(auto & save : db->getEditableGlobalSimulationSaves()){
        repairDuplicateId(save);
        save->repairDatabase();
        dialog.setValue(dialog.value() + 1);
        QCoreApplication::processEvents();
    }

    const bool saved = db->writeToJson();
    dialog.setValue(dialog.value() + 1);

    QMessageBox::information(nullptr, QObject::tr("Naprawa bazy danych"),
                             saved
                                 ? QObject::tr("Sprawdzono bazę danych i zapisano naprawione duplikaty identyfikatorów.")
                                 : QObject::tr("Sprawdzono bazę danych, ale nie udało się zapisać wszystkich zmian."),
                             QMessageBox::Ok);
}

void GlobalDatabase::repairSimulationSaves()
{
    GlobalDatabase * db = this;
    if(db->getLoadedSimulationSaves() == false){
        db->loadSimulationSaves(true);
        db->setLoadedSimulationSaves(true);
    }

    QProgressDialog dialog;
    dialog.setStyleSheet("QProgressDialog{background-color: white; color: black;}");
    dialog.setMinimum(0);
    dialog.setMaximum(db->getEditableGlobalSimulationSaves().count() * 11);
    dialog.setMinimumDuration(0);
    dialog.setValue(0);
    dialog.setWindowTitle(QObject::tr("Naprawa zapisów symulacji "));
        //dialog.setLabelText(QString(QObject::tr("Postęp naprawiania zapisów symulacji: %1 z %2")).arg(QString::number(dialog.value())).arg(QString::number(dialog.maximum())));
        dialog.setWindowModality(Qt::WindowModal);

    int skippedSaves = 0;
    int failedSaves = 0;
    for(auto & save : GlobalDatabase::get()->getEditableGlobalSimulationSaves())
    {
        if(save == nullptr || save->getActualSeason() == nullptr
            || save->getActualSeason()->getActualCalendar() == nullptr
            || save->getHillsReference().isEmpty())
        {
            skippedSaves++;
            dialog.setValue(dialog.value() + 11);
            QCoreApplication::processEvents();
            continue;
        }

        SeasonCalendar * calendar = save->getActualSeason()->getActualCalendar();
        calendar->fixAdvancementClassifications();
        dialog.setValue(dialog.value() + 1);
        QCoreApplication::processEvents();
        calendar->fixAdvancementCompetitions();
        dialog.setValue(dialog.value() + 1);
        QCoreApplication::processEvents();
        calendar->fixCompetitionsClassifications();
        dialog.setValue(dialog.value() + 1);
        QCoreApplication::processEvents();
        calendar->fixCompetitionsHills(&save->getHillsReference(), save->getHillsReference().first());
        dialog.setValue(dialog.value() + 1);
        QCoreApplication::processEvents();
        save->repairDatabase();
        dialog.setValue(dialog.value() + 5);
        QCoreApplication::processEvents();
        if(!save->saveToFile(UserDataPaths::path("Saves/")))
            failedSaves++;
        dialog.setValue(dialog.value() + 2);
        QCoreApplication::processEvents();
    }
    QMessageBox::information(nullptr, QObject::tr("Naprawiono zapisy symulacji"),
                             QObject::tr("Zakończono naprawę zapisów symulacji. Pominięto: %1, błędy zapisu: %2.")
                                 .arg(skippedSaves).arg(failedSaves), QMessageBox::Ok);
}

QVector<Country> GlobalDatabase::getCountries() const
{
    return countries;
}

QVector<Country> &GlobalDatabase::getCountriesReference()
{
    return countries;
}

void GlobalDatabase::setCountries(const QVector<Country> &newCountries)
{
    countries = newCountries;
}

QVector<JumpsImportancePreset> GlobalDatabase::getJumpsImportancePresets() const
{
    return jumpsImportancePresets;
}

QVector<JumpsImportancePreset> &GlobalDatabase::getJumpsImportancePresetsReference()
{
    return jumpsImportancePresets;
}

void GlobalDatabase::setJumpsImportancePresets(const QVector<JumpsImportancePreset> &newJumpsImportancePresets)
{
    jumpsImportancePresets = newJumpsImportancePresets;
}

QVector<JumperFormGeneratorSettingsPreset> GlobalDatabase::getFormGeneratorPresets() const
{
    return formGeneratorPresets;
}

QVector<JumperFormGeneratorSettingsPreset> &GlobalDatabase::getEditableFormGeneratorPresets()
{
    return formGeneratorPresets;
}

void GlobalDatabase::setFormGeneratorPresets(const QVector<JumperFormGeneratorSettingsPreset> &newFormGeneratorPresets)
{
    formGeneratorPresets = newFormGeneratorPresets;
}

bool GlobalDatabase::getLoadedSimulationSaves() const
{
    return loadedSimulationSaves;
}

void GlobalDatabase::setLoadedSimulationSaves(bool newLoadedSimulationSaves)
{
    loadedSimulationSaves = newLoadedSimulationSaves;
}

QVector<SeasonCalendarPreset> GlobalDatabase::getGlobalCalendarPresets() const
{
    return globalCalendarPresets;
}

QVector<SeasonCalendarPreset> &GlobalDatabase::getEditableGlobalCalendarPresets()
{
    return globalCalendarPresets;
}

void GlobalDatabase::setGlobalCalendarPresets(const QVector<SeasonCalendarPreset> &newGlobalCalendarPresets)
{
    globalCalendarPresets = newGlobalCalendarPresets;
}

QVector<PointsForPlacesPreset> GlobalDatabase::getGlobalPointsForPlacesPresets() const
{
    return globalPointsForPlacesPresets;
}

QVector<PointsForPlacesPreset> &GlobalDatabase::getEditableGlobalPointsForPlacesPresets()
{
    return globalPointsForPlacesPresets;
}

void GlobalDatabase::setGlobalPointsForPlacesPresets(const QVector<PointsForPlacesPreset> &newGlobalPointsForPlacesPresets)
{
    globalPointsForPlacesPresets = newGlobalPointsForPlacesPresets;
}

QVector<SimulationSave *> GlobalDatabase::getGlobalSimulationSaves() const
{
    return globalSimulationSaves;
}

QVector<SimulationSave *> &GlobalDatabase::getEditableGlobalSimulationSaves()
{
    return globalSimulationSaves;
}

void GlobalDatabase::setGlobalSimulationSaves(const QVector<SimulationSave *> &newGlobalSimulationSaves)
{
    globalSimulationSaves = newGlobalSimulationSaves;
}

QVector<CompetitionRules> GlobalDatabase::getGlobalCompetitionsRules() const
{
    return globalCompetitionsRules;
}

void GlobalDatabase::setGlobalCompetitionsRules(const QVector<CompetitionRules> &newGlobalCompetitionsRules)
{
    globalCompetitionsRules = newGlobalCompetitionsRules;
}

QVector<CompetitionRules> &GlobalDatabase::getEditableCompetitionRules()
{
    return globalCompetitionsRules;
}

QVector<Hill> GlobalDatabase::getGlobalHills() const
{
    return globalHills;
}

void GlobalDatabase::setGlobalHills(const QVector<Hill> &newGlobalHills)
{
    globalHills = newGlobalHills;
}

QVector<Hill> &GlobalDatabase::getEditableGlobalHills()
{
    return globalHills;
}

QVector<Jumper> GlobalDatabase::getGlobalJumpers() const
{
    return globalJumpers;
}

void GlobalDatabase::setGlobalJumpers(const QVector<Jumper> &newGlobalJumpers)
{
    globalJumpers = newGlobalJumpers;
}

QVector<Jumper> &GlobalDatabase::getEditableGlobalJumpers()
{
    return globalJumpers;
}

GlobalDatabase *GlobalDatabase::get()
{
    static GlobalDatabase globalDatabase;
    return &globalDatabase;
}

void GlobalDatabase::removeJumper(int index)
{
    globalJumpers.remove(index);
}

bool GlobalDatabase::loadFromJson()
{
    packsWritable=false;
    bool ok=true;
    const auto record=[&](bool loaded){ok=loaded && ok;};
    record(loadPacks());record(loadPointsForPlacesPresets());
    record(loadCalendarPresets());record(loadFormGeneratorPresets());
    record(loadJumpsImportancePresets());record(loadCountries());
    return ok;
}

bool GlobalDatabase::writeToJson()
{
    writeFailures.clear();
    const auto record=[this](const QString &name,bool ok){if(!ok)writeFailures.append(name);};
    record("Packs/Rules",writePacks());
    record(UserDataPaths::path("Saves/"),writeSimulationSaves());
    record("Packs/Points",writePointsForPlacesPresets());
    record("Calendars",writeCalendarPresets());
    record("formGeneratorPresets/",writeFormGeneratorPresets());
    record("jumpsImportancePresets/",writeJumpsImportancePresets());
    record("globalCountries.json",writeCountries());
    if(!writeFailures.isEmpty())QMessageBox::critical(nullptr,QObject::tr("Błąd zapisu danych"),
        QObject::tr("Nie zapisano części danych:\n%1").arg(writeFailures.join('\n')));
    return writeFailures.isEmpty();
}

bool GlobalDatabase::loadJumpers()
{
    return loadPacks();
}

bool GlobalDatabase::loadHills()
{
    return loadPacks();
}

bool GlobalDatabase::loadCompetitionsRules()
{
    return loadPacks();
}

bool GlobalDatabase::loadSimulationSaves(bool progressDialog)
{
    globalSimulationSaves.clear();

    QDir dir;
    dir.setPath(UserDataPaths::path("Saves"));
    dir.setNameFilters(QStringList() << "*.json");

    const QStringList fileNames = dir.entryList();
    QStringList filePaths;
    filePaths.reserve(fileNames.count());
    for(const QString &fileName : fileNames)
        filePaths.append(dir.absoluteFilePath(fileName));

    bool ok=true;int failedSaves=0;
    for(const auto &path:filePaths){
        if(auto *entry=SimulationSave::loadIndexEntry(path))globalSimulationSaves.append(entry);
        else ++failedSaves;
    }

    SimulationSaveListOrder::restore(globalSimulationSaves);
    if(failedSaves==0)SimulationSaveListOrder::remember(globalSimulationSaves);

    if(failedSaves > 0)
    {
        ok = false;
        QMessageBox::warning(nullptr,
                             QObject::tr("Wczytywanie zapisów symulacji"),
                             QObject::tr("Nie udało się wczytać %1 plików zapisów. Pliki są niedostępne albo uszkodzone.")
                                 .arg(failedSaves));
    }
    return ok;
}

bool GlobalDatabase::loadPointsForPlacesPresets()
{
    globalPointsForPlacesPresets.clear();

    QDir dir;
    dir.setPath(UserDataPaths::path("Packs/Points"));
    dir.setNameFilters(QStringList() << "*.json");
    QStringList fileNames = dir.entryList();

    bool ok = true;

    QFuture<PointsForPlacesPreset> future = QtConcurrent::mapped(fileNames, PointsForPlacesPreset::loadFromJson);

    future.waitForFinished();
    globalPointsForPlacesPresets = future.results().toVector();
    return ok;
}

bool GlobalDatabase::loadCalendarPresets(bool before1_2_0)
{
    globalCalendarPresets.clear();
    QDir dir(UserDataPaths::path("Calendars"));
    dir.setNameFilters(QStringList() << "*.json");

    QStringList fileNames = dir.entryList();

    bool ok = true;
    QStringList failures;
    for(auto & fileName : fileNames){
        QFile file(dir.absoluteFilePath(fileName));
        QString error;
        if(!file.open(QFile::ReadOnly | QFile::Text))
        {
            ok = false;
            failures.append(fileName+": "+file.errorString());
            invalidCalendarPresetPaths.insert(file.fileName().toCaseFolded());
            continue;
        }
        QJsonParseError parse;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(),&parse);
        QJsonObject object = doc.object().value("calendar-preset").toObject();
        if(parse.error!=QJsonParseError::NoError || !doc.isObject() || !SeasonCalendarPreset::validateJson(object,error,before1_2_0)) {
            ok=false;failures.append(fileName+": "+(parse.error!=QJsonParseError::NoError?parse.errorString():error));
            invalidCalendarPresetPaths.insert(file.fileName().toCaseFolded());
            continue;
        }
        invalidCalendarPresetPaths.remove(file.fileName().toCaseFolded());

        QMap<ulong, Identifiable*> objects;
        auto o = &objects;
        if(!before1_2_0) o = nullptr;
        IdentifiableObjectsStorage localStorage;
        IdentifiableObjectsStorage * storage = &localStorage;
        if(before1_2_0)
            storage = nullptr;
        SeasonCalendarPreset p = SeasonCalendarPreset::getFromJson(object, storage, o);
        globalCalendarPresets.push_back(p);
        file.close();
    }
    if(!failures.isEmpty())QMessageBox::warning(nullptr,QObject::tr("Preset kalendarza"),
        QObject::tr("Pominięto nieprawidłowe presety kalendarza. Pliki pozostały bez zmian:\n%1").arg(failures.join('\n')));
    return ok;
}

bool GlobalDatabase::loadFormGeneratorPresets()
{
    formGeneratorPresets.clear();
    QDir dir;
    dir.setPath(UserDataPaths::path("Tools/FormPresets"));
    dir.setNameFilters(QStringList() << "*.json");

    QStringList fileNames = dir.entryList();

    bool ok = true;
    for(auto & fileName : fileNames){
        QFile file(UserDataPaths::path("Tools/FormPresets/") + fileName);
        if(!file.open(QFile::ReadOnly | QFile::Text))
        {
            QMessageBox message(QMessageBox::Icon::Critical, QObject::tr("Nie można otworzyć pliku z presetem generatora formy"), QObject::tr("Nie udało się otworzyć presetu generatora formy: ") + fileName +QObject::tr("\nUpewnij się, że istnieje tam taki plik lub ma on odpowiednie uprawnienia"),  QMessageBox::StandardButton::Ok);
            message.setModal(true);
            message.exec();
            ok = false;
        }
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject object = doc.object().value("jumper-form-generator-settings-preset").toObject();
        JumperFormGeneratorSettingsPreset p = JumperFormGeneratorSettingsPreset::getFromJson(object);
        formGeneratorPresets.push_back(p);
        file.close();
    }
    return ok;
}

bool GlobalDatabase::loadJumpsImportancePresets()
{
    jumpsImportancePresets.clear();
    QDir dir;
    dir.setPath(UserDataPaths::path("Tools/ImportancePresets"));
    dir.setNameFilters(QStringList() << "*.json");

    QStringList fileNames = dir.entryList();

    bool ok = true;
    for(auto & fileName : fileNames){
        QFile file(UserDataPaths::path("Tools/ImportancePresets/") + fileName);
        file.open(QFile::ReadOnly | QFile::Text);
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject object = doc.object();
        JumpsImportancePreset p = JumpsImportancePreset::getFromJson(object);
        jumpsImportancePresets.push_back(p);
        file.close();
    }
    return ok;
}

bool GlobalDatabase::loadCountries()
{
    countries.clear();
    QFile file(UserDataPaths::path("Settings/countries.json"));
    file.open(QFile::ReadOnly | QFile::Text);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject object = doc.object();
    QJsonArray array = object.value("countries").toArray();
    for(auto val : array)
    {
        Country c;
        c.setName(val.toObject().value("name").toString());
        c.setAlpha2(val.toObject().value("alpha2").toString());
        c.setAlpha3(val.toObject().value("alpha3").toString());
        countries.push_back(c);
    }
    file.close();
    return true;
}

bool GlobalDatabase::writeJumpers()
{
    return writePacks();
}

bool GlobalDatabase::writeHills()
{
    return writePacks();
}

bool GlobalDatabase::writeCompetitionsRules()
{
    return writePacks();
}

bool GlobalDatabase::writeSimulationSaves()
{
    bool ok = true;
    /*QtConcurrent::map(globalSimulationSaves, [](SimulationSave * save){
        save->saveToFile(UserDataPaths::path("Saves/"));
    });*/
 //&SimulationSave::saveToFile);

    qDebug()<<"saves";
    for(auto & save : globalSimulationSaves)
    {
        if(save->isLoaded() && save->saveToFile(UserDataPaths::path("Saves/")) == false)
            ok = false;
    }
    return ok;
}

bool GlobalDatabase::writePointsForPlacesPresets()
{
    bool ok = true;
    for(auto & preset : globalPointsForPlacesPresets)
    {
        if(preset.saveToFile(UserDataPaths::path("Packs/Points/")) == false)
            ok = false;
    }
    return ok;
}

bool GlobalDatabase::writeCalendarPresets()
{
    bool ok = true;
    for(auto & preset : globalCalendarPresets)
    {
        const auto destination=QFileInfo(UserDataPaths::path("Calendars/")+preset.getName()+".json").absoluteFilePath().toCaseFolded();
        if(invalidCalendarPresetPaths.contains(destination)){ok=false;continue;}
        if(preset.saveToFile(UserDataPaths::path("Calendars/")) == false)
            ok = false;
    }
    return ok;
}

bool GlobalDatabase::writeFormGeneratorPresets()
{
    bool ok = true;
    for(auto & preset : formGeneratorPresets)
    {
        if(preset.saveToFile(UserDataPaths::path("Tools/FormPresets/")) == false)
            ok = false;
    }
    return ok;
}

bool GlobalDatabase::writeJumpsImportancePresets()
{
    bool ok = true;
    for(auto & preset : jumpsImportancePresets)
    {
        if(preset.saveToFile(UserDataPaths::path("Tools/ImportancePresets/")) == false)
            ok = false;
    }
    return ok;
}

bool GlobalDatabase::writeCountries()
{
    QJsonDocument document;
    QJsonObject mainObject;
    QJsonArray array;
    for(auto & country : countries)
    {
        QJsonObject countryObject;
        countryObject.insert("name", country.getName());
        countryObject.insert("alpha2", country.getAlpha2());
        countryObject.insert("alpha3", country.getAlpha3());
        array.push_back(countryObject);
    }
    mainObject.insert("countries", array);
    document.setObject(mainObject);
    return AtomicJsonFile::write(UserDataPaths::path("Settings/countries.json"),document);
}


Country * GlobalDatabase::getCountryByAlpha2(QString alpha2)
{
    for(auto & c : countries)
    {
        if(c.getAlpha2() == alpha2)
            return &c;
    }
    return new Country("", "xx", "xxx");
}

Country * GlobalDatabase::getCountryByAlpha3(QString alpha3)
{
    alpha3 = CountryCodes::toFis(alpha3);
    for(auto & c : countries)
    {
        if(c.getAlpha3() == alpha3)
            return &c;
    }
    return new Country("", "xx", "xxx");
}

void GlobalDatabase::ensurePacks()
{
    if (!packs.isEmpty() || packsWritable) return;
    for (int i = 0; i < 3; ++i) {
        DataPack pack;
        pack.kind = DataPack::Kind(i);
        pack.name = QObject::tr("Domyślna");
        activePackIds[i] = pack.id;
        packs.append(pack);
    }
    syncActivePacks();
}

DataPack *GlobalDatabase::activePack(DataPack::Kind kind)
{
    ensurePacks();
    for (auto &pack : packs) if (pack.id == activePackIds[int(kind)]) return &pack;
    return nullptr;
}

void GlobalDatabase::syncActivePacks()
{
    for (auto &p : packs) {
        if (p.id != activePackIds[int(p.kind)]) continue;
        if (p.kind == DataPack::Jumpers) p.jumpers = globalJumpers;
        else if (p.kind == DataPack::Hills) p.hills = globalHills;
        else { p.rules = globalCompetitionsRules; p.points = globalPointsForPlacesPresets; }
    }
}

bool GlobalDatabase::selectPack(const QString &id)
{
    syncActivePacks();
    for(const auto &pack:packs)if(pack.id==id) {
        if(pack.kind==DataPack::Rules) {
            if(!writePacks())return false;
            SimulationV2::SimulationCatalog catalog;auto snapshot=catalog.load({},100,{});QString error;
            if(!catalog.select(snapshot,pack.kind,id,error))return false;
        }
        activePackIds[int(pack.kind)]=id;
        if(pack.kind==DataPack::Jumpers)globalJumpers=pack.jumpers;
        else if(pack.kind==DataPack::Hills)globalHills=pack.hills;
        else globalCompetitionsRules=pack.rules;
        return true;
    }
    return false;
}

QString GlobalDatabase::addPack(DataPack pack)
{
    ensurePacks();
    syncActivePacks();
    // Imports are independent copies, including when importing the same file twice.
    pack.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    packs.append(pack);
    selectPack(pack.id);
    return pack.id;
}

QJsonObject GlobalDatabase::packsSnapshot()
{
    ensurePacks();
    syncActivePacks();
    QJsonArray list, active;
    for (const auto &p : packs) list.append(p.toJson());
    for (const auto &id : activePackIds) active.append(id);
    return QJsonObject{{"version", 1}, {"packs", list}, {"active", active}};
}

bool GlobalDatabase::restorePacks(const QJsonObject &o, QString &error)
{
    QVector<DataPack> loaded;
    QSet<QString> ids;
    if (o.value("version").toInt() != 1 || !o.value("packs").isArray() || o.value("active").toArray().size() != 3) {
        error = QObject::tr("Nieprawidłowy katalog paczek."); return false;
    }
    for (const auto &v : o.value("packs").toArray()) {
        DataPack p;
        if (!DataPack::fromJson(v.toObject(), p, error)) return false;
        if (ids.contains(p.id)) { error = QObject::tr("Powtórzony identyfikator paczki."); return false; }
        ids.insert(p.id); loaded.append(p);
    }
    const auto active = o.value("active").toArray();
    for (int kind = 0; kind < 3; ++kind) {
        bool found = false;
        for (const auto &p : loaded) if (p.kind == kind && p.id == active[kind].toString()) found = true;
        bool hasKind=false;for(const auto &pack:loaded)if(int(pack.kind)==kind)hasKind=true;
        if (!found && (hasKind || !active[kind].toString().isEmpty())) { error = QObject::tr("Brak aktywnej paczki."); return false; }
    }
    packs = loaded;
    globalJumpers.clear();globalHills.clear();globalCompetitionsRules.clear();
    // Assign directly: switching here would copy the old working vectors over loaded packs.
    for (int i = 0; i < 3; ++i) activePackIds[i] = active[i].toString();
    for (const auto &p : packs) if (p.id == activePackIds[int(p.kind)]) {
        if (p.kind == DataPack::Jumpers) globalJumpers = p.jumpers;
        else if (p.kind == DataPack::Hills) globalHills = p.hills;
        else { globalCompetitionsRules = p.rules; }
    }
    error.clear();
    return true;
}

bool GlobalDatabase::loadPacks()
{
    const auto snapshot=SimulationV2::SimulationCatalog().load({},100,{});
    QJsonArray entries,active;
    for(const auto &entry:snapshot.packs)entries.append(entry.data.toJson());
    for(const auto &id:snapshot.active)active.append(id);
    QString error;const bool ok=restorePacks({{"version",1},{"packs",entries},{"active",active}},error);
    packsWritable=ok;
    if(!ok)QMessageBox::critical(nullptr,QObject::tr("Biblioteka"),QObject::tr("Nie udało się wczytać paczek z folderu danych.\n%1").arg(error));
    return ok;
}

bool GlobalDatabase::writePacks()
{
    if(!packsWritable)return false;
    syncActivePacks();
    for(const auto &pack:packs)if(pack.kind==DataPack::Rules) {
        auto object=pack.toJson();object["pointsForPlacesPresets"]=QJsonArray();
        if(!AtomicJsonFile::write(UserDataPaths::path("Packs/Rules/"+pack.id+".json"),QJsonDocument(object)))return false;
    }
    return true;
}
