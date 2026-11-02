#pragma once
#include "../source/global/JsonGraphValidation.h"
#include "../source/global/AtomicJsonFile.h"

static void productionPersistenceRegressions() {
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
    QDir().mkdir("blocked.json");check(!AtomicJsonFile::write("blocked.json",original,false) && QFileInfo("blocked.json").isDir(),"atomic write failure is reported without replacing the destination");
    QDir::setCurrent(previousDirectory);

    SimulationSave source;source.setEngineVersion(1);
    auto *hill=new Hill;auto *jumper=new Jumper("Test","Jumper","POL");source.setHills({hill});source.setJumpers({jumper});
    auto *calendar=new SeasonCalendar("Test");auto *first=new CompetitionInfo(hill),*second=new CompetitionInfo(hill);
    second->setRules(first->getRules());calendar->setCompetitions({first,second});
    Season season;season.setCalendars({calendar});season.setActualCalendar(calendar);source.setSeasons({season});source.setActualSeason(&source.getSeasonsReference().first());
    const auto valid=SimulationSave::getJsonObject(source);QString error;
    check(JsonGraphValidation::validateSave(valid,error),"shared inline rules remain compatible with saved calendars");
    std::unique_ptr<SimulationSave> loaded(SimulationSave::getFromJson(valid,nullptr));check(bool(loaded),"valid save still deserializes");
    auto duplicate=valid;auto jumpers=valid.value("jumpers").toArray();jumpers.append(jumpers.first());duplicate["jumpers"]=jumpers;
    check(!SimulationSave::getFromJson(duplicate,nullptr),"duplicate jumper identities are rejected before creating objects");
    duplicate=valid;auto hills=valid.value("hills").toArray();auto badHill=hills.first().toObject();badHill["id"]=jumper->getIDStr();hills[0]=badHill;duplicate["hills"]=hills;
    check(!SimulationSave::getFromJson(duplicate,nullptr),"cross-type UUID collisions are rejected before creating objects");
    duplicate=valid;auto seasons=valid.value("seasons").toArray();auto badSeason=seasons.first().toObject();badSeason["actual-calendar-id"]=hill->getIDStr();seasons[0]=badSeason;duplicate["seasons"]=seasons;
    check(!SimulationSave::getFromJson(duplicate,nullptr),"references to an existing object of the wrong type are rejected");
    qInfo()<<"PASS: production persistence defaults and UUID validation";
}
