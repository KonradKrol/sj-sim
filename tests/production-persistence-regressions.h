#pragma once
#include "../source/global/JsonGraphValidation.h"
#include "../source/global/AtomicJsonFile.h"
#include "../source/global/JsonImportError.h"
#include <QLockFile>
#include "../source/UI/DatabaseEditor/ListModels/SeasonJumpersListModel.h"
#include "../source/UI/DatabaseEditor/ListModels/SeasonHillsListModel.h"

static void productionPersistenceRegressions() {
    {
        Jumper a("Model","Athlete","POL"),b("Other","Athlete","NOR");Hill hill;
        QVector<Jumper *> actors{&a,&b};QVector<Hill *> hills{&hill};
        SeasonJumpersListModel actorModel(&actors);SeasonHillsListModel hillModel(&hills);
        QAbstractItemModelTester actorContract(&actorModel,QAbstractItemModelTester::FailureReportingMode::Fatal),hillContract(&hillModel,QAbstractItemModelTester::FailureReportingMode::Fatal);
        check(!actorModel.insertRows(0,1) && !hillModel.insertRows(0,1),"season models require complete objects for insertion");
        check(actorModel.removeRows(0,2) && actors.isEmpty() && hillModel.removeRows(0,1) && hills.isEmpty(),"season model removals update containers before publishing row removal");
        check(!actorModel.removeRows(0,1) && !hillModel.removeRows(-1,1),"invalid season row removals are rejected");
        check(actorModel.insertItem(0,&a) && hillModel.insertItem(0,&hill),"season models publish complete inserted objects");
        check(!actorModel.data(actorModel.index(5)).isValid(),"out-of-range data requests are safe");
    }
    auto *settings=GlobalAppSettings::get();
    check(settings->getLiveCompetition() && settings->getLiveCompetitionSpeedMulti()==1
        && settings->getShowCalendarEditorHelp() && settings->getShowSeasonJumpersAndHillsHelp(),"application settings have deterministic safe defaults before loading");
    const auto choices=JumpDataInfoChoice::getJsonObject(JumpDataInfoChoice());
    for(const auto &value:choices)check(value.isBool() && !value.toBool(),"all jump detail flags are initialized");

    const auto previousDirectory=QDir::currentPath();QTemporaryDir disk;check(disk.isValid(),"isolated persistence fixture");QDir::setCurrent(disk.path());
    QTimer dismiss;QObject::connect(&dismiss,&QTimer::timeout,[]{for(auto *widget:QApplication::allWidgets())if(auto *message=qobject_cast<QMessageBox *>(widget))message->accept();});dismiss.start(10);
    check(!settings->loadFromJson() && settings->getLiveCompetitionSpeedMulti()==1,"missing settings retain safe defaults");
    QDir().mkpath("userData/Settings");{QFile broken("userData/Settings/appSettings.json");check(broken.open(QIODevice::WriteOnly),"create malformed settings fixture");broken.write("{");}
    check(!settings->loadFromJson() && settings->getLiveCompetitionSpeedMulti()==1,"malformed settings retain safe defaults");dismiss.stop();
    const QJsonDocument original(QJsonObject{{"value",1}}),replacement(QJsonObject{{"value",2}});
    check(AtomicJsonFile::write("atomic.json",original,false) && AtomicJsonFile::write("atomic.json",replacement,false),"atomic JSON can create and replace a document");
    {QFile stored("atomic.json");check(stored.open(QIODevice::ReadOnly) && QJsonDocument::fromJson(stored.readAll()).object().value("value").toInt()==2,"committed JSON contains the complete replacement");}
    for(const auto &name:QStringList{"../outside","..\\outside","/tmp/outside","",":bad","a/b","a\\b"})
        check(AtomicJsonFile::namedPath(disk.path(),name).isEmpty(),"preset names cannot escape their destination directory");
    check(AtomicJsonFile::namedPath(disk.path(),QString::fromUtf8("Puchar Świata"))==disk.filePath(QString::fromUtf8("Puchar Świata.json")),"safe localized preset names are preserved");
    {QFile victim(disk.filePath("victim.json"));check(victim.open(QIODevice::WriteOnly),"create symlink target fixture");victim.write("preserved");}
    if(QFile::link(disk.filePath("victim.json"),disk.filePath("alias.json")))
        check(AtomicJsonFile::namedPath(disk.path(),"alias").isEmpty(),"preset writes cannot replace a symlink target");
    QDir().mkdir("blocked.json");check(!AtomicJsonFile::write("blocked.json",original,false) && QFileInfo("blocked.json").isDir(),"atomic write failure is reported without replacing the destination");
    QDir().mkpath("userData/GlobalDatabase");dismiss.start(10);
    auto *database=GlobalDatabase::get();check(!database->writeToJson(),"save-all reports a protected pack catalog failure");dismiss.stop();
    check(database->lastWriteFailures()==QStringList{"packs.json"} && QFile::exists("userData/GlobalDatabase/globalCountries.json")
        && QFile::exists("userData/GlobalDatabase/globalJumpers.json"),"a pack catalog failure does not skip independent database writes");
    QDir::setCurrent(previousDirectory);

    SimulationSave source;source.setEngineVersion(1);
    auto *hill=new Hill;hill->setName("Test Hill");hill->setKPoint(120);hill->setHSPoint(134);auto *jumper=new Jumper("Test","Jumper","POL");source.setHills({hill});source.setJumpers({jumper});
    auto *calendar=new SeasonCalendar("Test");auto *first=new CompetitionInfo(hill),*second=new CompetitionInfo(hill);
    first->getRulesPointer()->setRounds({RoundInfo(10,true)});second->setRules(first->getRules());calendar->setCompetitions({first,second});
    Season season;season.setCalendars({calendar});season.setActualCalendar(calendar);source.setSeasons({season});source.setActualSeason(&source.getSeasonsReference().first());
    const auto valid=SimulationSave::getJsonObject(source);QString error;
    check(JsonGraphValidation::validateSave(valid,error),"shared inline rules remain compatible with saved calendars");
    for(const auto &key:QStringList{"jumpers","hills","rules","seasons"}) {
        for(const auto &wrong:QList<QJsonValue>{QJsonValue(123),QJsonValue("broken"),QJsonValue(QJsonValue::Null),QJsonValue(QJsonValue::Undefined)}) {
            auto malformed=valid;malformed.insert(key,wrong);
            check(!JsonGraphValidation::validateSave(malformed,error),"malformed required save collections cannot silently become empty");
        }
    }
    for(const auto &key:QStringList{"competitions","classifications"}) {
        auto malformed=valid;auto seasons=malformed["seasons"].toArray();auto entry=seasons[0].toObject();auto calendars=entry["calendars"].toArray();auto calendarObject=calendars[0].toObject();
        calendarObject[key]=123;calendars[0]=calendarObject;entry["calendars"]=calendars;seasons[0]=entry;malformed["seasons"]=seasons;
        check(!JsonGraphValidation::validateSave(malformed,error),"malformed calendar collections are rejected before allocation");
    }
    {
        auto cross=valid;auto seasons=cross["seasons"].toArray();auto firstSeason=seasons[0].toObject();auto calendars=firstSeason["calendars"].toArray();auto firstCalendar=calendars[0].toObject();
        const auto calendarId=QString::fromStdString(Uuid::v4().str());
        QJsonObject otherCalendar{{"id",calendarId},{"competitions",QJsonArray()},{"classifications",QJsonArray()}};
        QJsonObject otherSeason{{"id",QString::fromStdString(Uuid::v4().str())},{"actual-calendar-id",calendarId},{"calendars",QJsonArray{otherCalendar}}};
        firstSeason["actual-calendar-id"]=calendarId;seasons[0]=firstSeason;seasons.append(otherSeason);cross["seasons"]=seasons;
        check(!JsonGraphValidation::validateSave(cross,error),"a season cannot use another season's calendar");
        firstSeason["actual-calendar-id"]=firstCalendar["id"];
        auto events=firstCalendar["competitions"].toArray();auto dependent=events[0].toObject();dependent["advancement-competition-id"]=events[1].toObject()["id"];
        otherCalendar["competitions"]=QJsonArray{events[1]};firstCalendar["competitions"]=QJsonArray{dependent};
        firstSeason["calendars"]=QJsonArray{firstCalendar};otherSeason["calendars"]=QJsonArray{otherCalendar};cross["seasons"]=QJsonArray{firstSeason,otherSeason};
        check(!JsonGraphValidation::validateSave(cross,error),"qualification sources must belong to the event calendar");
    }
    std::unique_ptr<SimulationSave> loaded(SimulationSave::getFromJson(valid,nullptr));check(bool(loaded),"valid save still deserializes");
    {
        SimulationSaveManagerWindow manager(&source);
        source.setAutosaveEnabled(true);manager.markSeasonChanged();
        auto *timer=manager.findChild<QTimer *>("seasonAutosaveTimer");check(timer,"season autosave timer exists");timer->stop();
        const auto originalId=hill->getID();hill->setID(jumper->getID());dismiss.start(10);
        check(!manager.saveSeasonChanges() && manager.hasUnsavedSeasonChanges() && timer->isActive(),"failed autosave retains dirty revision and schedules another attempt");
        hill->setID(originalId);source.setAutosaveEnabled(false);timer->stop();dismiss.stop();
    }
    auto missing=valid;missing["actual-season-id"]=QString::fromStdString(Uuid::v4().str());
    check(!SimulationSave::getFromJson(missing,nullptr),"missing active season cannot silently become an empty save");
    missing=valid;missing["jumpers-lists"]=QJsonArray{QJsonObject{{"jumpers-ids",QJsonArray{QString::fromStdString(Uuid::v4().str())}}}};
    check(!SimulationSave::getFromJson(missing,nullptr),"missing roster participants are rejected before deserialization");
    auto balance=valid;auto balanceSeasons=balance["seasons"].toArray();auto balanceSeason=balanceSeasons[0].toObject();auto balanceCalendars=balanceSeason["calendars"].toArray();auto balanceCalendar=balanceCalendars[0].toObject();auto balanceEvents=balanceCalendar["competitions"].toArray();auto balanceEvent=balanceEvents[0].toObject();balanceEvent["simulation-balance"]="broken";balanceEvents[0]=balanceEvent;balanceCalendar["competitions"]=balanceEvents;balanceCalendars[0]=balanceCalendar;balanceSeason["calendars"]=balanceCalendars;balanceSeasons[0]=balanceSeason;balance["seasons"]=balanceSeasons;
    check(!SimulationSave::getFromJson(balance,nullptr),"invalid saved balance cannot silently change an ongoing competition");
    {
        QLockFile owner(disk.filePath("instance.lock")),other(disk.filePath("instance.lock"));
        check(owner.tryLock(0) && !other.tryLock(0),"only one writer can own a portable data directory");
        owner.unlock();check(other.tryLock(0),"closing the owner releases its data directory");
    }
    QJsonParseError parse;const QByteArray badJson="{\n  x}";QJsonDocument::fromJson(badJson,&parse);
    const auto importError=JsonImportError::file("/packs/broken.json",JsonImportError::syntax(badJson,parse));
    check(importError.contains("broken.json") && importError.contains("2"),"import errors identify the file and JSON line");
    {
        Team squad("POL");squad.setJumpers({jumper});
        for(auto *event:{first,second}) {
            event->getRulesPointer()->setCompetitionType(CompetitionRules::Team);event->getRulesPointer()->setJumpersInTeamCount(1);
            event->setTeams({squad});event->setPlayed(true);
            CompetitionSingleResult result(event,&event->getTeamsReference().first());
            event->getResultsReference().setResults({result});
        }
        std::unique_ptr<SimulationSave> teamSave(SimulationSave::getFromJson(SimulationSave::getJsonObject(source),nullptr));
        check(bool(teamSave),"team event snapshots roundtrip");
        for(auto *event:teamSave->getActualSeason()->getActualCalendar()->getCompetitions())
            check(event->getResultsReference().getResultsReference().first().getTeam()==&event->getTeamsReference().first(),"each team result belongs to its own event snapshot, even with repeated team IDs");
        for(auto *event:{first,second}) {event->setPlayed(false);event->getResultsReference().getResultsReference().clear();event->getTeamsReference().clear();event->getRulesPointer()->setCompetitionType(CompetitionRules::Individual);}
    }
    auto duplicate=valid;auto jumpers=valid.value("jumpers").toArray();jumpers.append(jumpers.first());duplicate["jumpers"]=jumpers;
    check(!SimulationSave::getFromJson(duplicate,nullptr),"duplicate jumper identities are rejected before creating objects");
    duplicate=valid;auto hills=valid.value("hills").toArray();auto badHill=hills.first().toObject();badHill["id"]=jumper->getIDStr();hills[0]=badHill;duplicate["hills"]=hills;
    check(!SimulationSave::getFromJson(duplicate,nullptr),"cross-type UUID collisions are rejected before creating objects");
    duplicate=valid;auto seasons=valid.value("seasons").toArray();auto badSeason=seasons.first().toObject();badSeason["actual-calendar-id"]=hill->getIDStr();seasons[0]=badSeason;duplicate["seasons"]=seasons;
    check(!SimulationSave::getFromJson(duplicate,nullptr),"references to an existing object of the wrong type are rejected");
    duplicate=valid;duplicate["jumpers-lists"]=QJsonArray{QJsonObject{{"jumpers-ids",QJsonArray{hill->getIDStr()}}}};
    check(!SimulationSave::getFromJson(duplicate,nullptr),"participant lists cannot cast a hill UUID to a jumper");
    check(source.saveToFile(disk.path(),"identity"),"write a valid isolated simulation save");
    QByteArray savedBytes;{QFile file(disk.filePath("identity.json"));check(file.open(QIODevice::ReadOnly),"read original save fixture");savedBytes=file.readAll();}
    const auto hillId=hill->getID();hill->setID(jumper->getID());dismiss.start(10);
    check(!source.saveToFile(disk.path(),"identity"),"invalid in-memory identities cannot overwrite an existing save");dismiss.stop();hill->setID(hillId);
    {QFile file(disk.filePath("identity.json"));check(file.open(QIODevice::ReadOnly) && file.readAll()==savedBytes,"identity validation preserves the previous valid save byte for byte");}
    SeasonCalendarPreset preset(calendar);preset.setName("Valid");preset.setHills({{hill->getName(),134},{hill->getName(),134}});
    const auto presetJson=SeasonCalendarPreset::getJsonObject(preset);
    check(SeasonCalendarPreset::validateJson(presetJson,error),"complete calendar preset is accepted");
    auto oldPreset=presetJson;oldPreset.remove("schema-version");check(SeasonCalendarPreset::validateJson(oldPreset,error),"unversioned legacy presets remain supported");
    auto oldCalendar=oldPreset.value("calendar").toObject();auto oldEvents=oldCalendar.value("competitions").toArray();
    for(int i=0;i<oldEvents.size();++i){auto event=oldEvents[i].toObject();for(const auto &key:{"rules","results"}){auto snapshot=event.value(key).toObject();snapshot["id"]="00000000-0000-0000-0000-000000000000";event[key]=snapshot;}oldEvents[i]=event;}
    oldCalendar["competitions"]=oldEvents;oldPreset["calendar"]=oldCalendar;
    check(SeasonCalendarPreset::validateJson(oldPreset,error),"legacy zero identities in empty preset snapshots are migrated safely");
    for(const auto &fileName:QDir(previousDirectory+"/userData/GlobalDatabase/calendarPresets").entryList({"*.json"},QDir::Files)) {
        QFile file(previousDirectory+"/userData/GlobalDatabase/calendarPresets/"+fileName);check(file.open(QIODevice::ReadOnly),"read shipped preset fixture");
        check(SeasonCalendarPreset::validateJson(QJsonDocument::fromJson(file.readAll()).object().value("calendar-preset").toObject(),error),"shipped preset remains compatible: "+fileName+": "+error);
    }
    auto invalidPreset=presetJson;invalidPreset["schema-version"]=2;check(!SeasonCalendarPreset::validateJson(invalidPreset,error),"unsupported preset versions are rejected");
    invalidPreset=presetJson;auto invalidCalendar=invalidPreset.value("calendar").toObject();invalidCalendar.remove("competitions");invalidPreset["calendar"]=invalidCalendar;
    check(!SeasonCalendarPreset::validateJson(invalidPreset,error),"missing event collections cannot become empty presets");
    invalidPreset=presetJson;invalidCalendar=presetJson.value("calendar").toObject();auto events=invalidCalendar.value("competitions").toArray();auto invalidEvent=events.first().toObject();invalidEvent["trial-round-id"]=source.getIDStr();events[0]=invalidEvent;invalidCalendar["competitions"]=events;invalidPreset["calendar"]=invalidCalendar;
    check(!SeasonCalendarPreset::validateJson(invalidPreset,error),"unresolved preset event links are rejected");
    QDir::setCurrent(disk.path());QDir().mkpath("userData/GlobalDatabase/calendarPresets");
    check(AtomicJsonFile::write("userData/GlobalDatabase/calendarPresets/Good.json",QJsonDocument(QJsonObject{{"calendar-preset",presetJson}}),false),"write isolated valid preset fixture");
    {QFile broken("userData/GlobalDatabase/calendarPresets/Valid.json");check(broken.open(QIODevice::WriteOnly),"create malformed preset fixture");broken.write("{");}
    dismiss.start(10);check(!database->loadCalendarPresets() && database->getGlobalCalendarPresets().size()==1,"preset library skips malformed JSON while loading valid files");dismiss.stop();
    check(!database->writeCalendarPresets(),"save-all cannot overwrite a skipped corrupt preset with another preset of the same name");
    {QFile broken("userData/GlobalDatabase/calendarPresets/Valid.json");check(broken.open(QIODevice::ReadOnly) && broken.readAll()=="{","malformed preset file remains untouched");}
    QDir::setCurrent(previousDirectory);
    qInfo()<<"PASS: production persistence defaults and UUID validation";
}
