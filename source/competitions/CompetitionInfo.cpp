#include "CompetitionInfo.h"

#include "../simulator/Hill.h"
#include "../global/IdentifiableObjectsStorage.h"
#include "../global/GlobalAppSettings.h"
#include "../global/GlobalDatabase.h"
#include "CompetitionRules.h"
#include "CompetitionResults.h"
#include "AbstractCompetitionManager.h"
#include "CompetitionManagers/IndividualCompetitionManager.h"
#include "CompetitionManagers/TeamCompetitionManager.h"
#include "../seasons/Season.h"

#include <QDate>
#include <QObject>
#include <QDebug>
#include <QDebug>
#include <QJsonDocument>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonArray>
#include <QMessageBox>
#include <QByteArray>
extern IdentifiableObjectsStorage seasonObjectsManager;
extern const QString appVersion;

CompetitionInfo::CompetitionInfo(Hill *hill) : hill(hill),
    Identifiable()
{
    exceptionalRoundsCount = (-1);
    cancelled = false;
    played = false;
    serieType = Competition;
    trialRound = nullptr;
    advancementClassification = nullptr;
    advancementCompetition = nullptr;
    results.setCompetition(this);
    jumpsImportance = 5;
}

CompetitionInfo::~CompetitionInfo()
{
    /*for(auto & t : trainings)
        if(t != nullptr)
            delete t;
    if(trialRound != nullptr)
        delete trialRound;*/
}

void CompetitionInfo::updateQualifyingCompetitions(SeasonCalendar *calendar)
{
    getQualifyingCompetitionsReference().clear();
    for(auto & comp : calendar->getCompetitionsReference())
    {
        if(this == comp->getAdvancementCompetition())
        {
            if(this->getQualifyingCompetitionsReference().contains(comp) == false)
                this->getQualifyingCompetitionsReference().push_back(comp);
        }
    }
}

bool CompetitionInfo::saveToJsonFile(QString dir, QString name)
{
    QDir d(dir);
    if(!d.exists())
        d.mkpath(".");
    QFile file(dir + name);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Nie można otworzyć pliku ", "Nie udało się otworzyć pliku " + dir + name+ "\nUpewnij się, że istnieją odpowiednie foldery lub aplikacja ma odpowiednie uprawnienia",  QMessageBox::StandardButton::Ok);
       qDebug()<<file.errorString();
 message.setModal(true);
        message.exec();
        return false;
    }
    QJsonDocument document;
    QJsonObject mainObject;
    mainObject.insert("competition", CompetitionInfo::getJsonObject(*this));
    document.setObject(mainObject);

    file.resize(0);
    file.write(document.toJson());
    file.close();
    return true;
}

bool CompetitionInfo::saveToCsvFile(QString dir, QString name)
{
    QDir d(dir);
    if(!d.exists())
        d.mkpath(".");
    QFile file(dir + name);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Nie można otworzyć pliku ", "Nie udało się otworzyć pliku " + dir + name+ "\nUpewnij się, że istnieją odpowiednie foldery lub aplikacja ma odpowiednie uprawnienia",  QMessageBox::StandardButton::Ok);
       qDebug()<<file.errorString();
 message.setModal(true);
        message.exec();
        return false;
    }
    QTextStream stream(&file);

    stream << QObject::tr("Poz.") <<";"<< QObject::tr("Zawodnik") <<";"<< QObject::tr("Kraj") <<";"<< QObject::tr("Nota laczna")<<";";
    int i=1;
    for(auto & round : rules.getRoundsReference())
    {
        stream << QObject::tr("Odleglosc ") + QString::number(i) <<";";
        stream << QObject::tr("Belka ") + QString::number(i) <<";";
        stream << QObject::tr("Wiatr ") + QString::number(i) <<";";
        stream << QObject::tr("Rekompensata ") + QString::number(i) <<";";
        stream << QObject::tr("Od sedziow ") + QString::number(i) <<";";
        stream << QObject::tr("Nota ") + QString::number(i) <<";";
        i++;
    }
    stream<<"\n";
    for(auto & res : results.getResultsReference())
    {
        stream << res.getCsvResultsObject() << "\n";
    }
    file.close();
    return true;
}

QVector<CompetitionInfo *> CompetitionInfo::getSpecificTypeMainCompetitions(QVector<CompetitionInfo *> & competitions, int type)
{
    QVector<CompetitionInfo *> toReturn;
    for(auto & competition : competitions)
    {
        if(competition->getSerieType() == CompetitionInfo::Qualifications || competition->getSerieType() == CompetitionInfo::Competition)
        {
            if(competition->getRulesPointer()->getCompetitionType() == type)
            {
                toReturn.push_back(competition);
            }
        }
    }

    return toReturn;
}

QVector<CompetitionInfo *> CompetitionInfo::getSpecificTypeCompetitions(QVector<CompetitionInfo *> &competitions, int type)
{
    QVector<CompetitionInfo *> toReturn;
    for(auto & competition : competitions)
    {
            if(competition->getRulesPointer()->getCompetitionType() == type)
            {
                toReturn.push_back(competition);
            }
    }

    return toReturn;
}

QVector<CompetitionInfo *> CompetitionInfo::getCompetitionsByStartAndEnd(QVector<CompetitionInfo *> competitions, CompetitionInfo *start, CompetitionInfo *end)
{
    qDebug()<<start->getHill()->getName()<<", "<<start;
    qDebug()<<end;
    qDebug()<<end->getHill()->getName()<<", "<<end;
    qDebug()<<"indexof start: "<<competitions.indexOf(start);
    qDebug()<<"indexof end: "<<competitions.indexOf(end);
    competitions.remove(0, competitions.indexOf(start));
    competitions.remove(competitions.indexOf(end) + 1, competitions.count() - (competitions.indexOf(end) + 1));

    //1 wisla (index 0)
    //2 ga-pa (index 1)
    //3 planica (index 2)
    //4 oslo (index 3)
    //- - - - - -
    //8 vikersund (index 7)
    return competitions;
}

QVector<CompetitionInfo *> CompetitionInfo::mergeSeasonsCompetitions(QVector<Season> *seasons, QString filter, bool mergeCalendars)
{
    QVector<CompetitionInfo *> competitions;
    for(auto & season : *seasons)
    {
        for(auto & cal : season.getCalendarsReference())
            if(cal->getName() == filter || filter == "" || mergeCalendars == true)
                for(auto & comp : cal->getCompetitionsReference())
                    competitions.push_back(comp);
    }
    return competitions;
}

int CompetitionInfo::howManyPlayedStatic(QVector<CompetitionInfo *> comps)
{
    int counter = 0;
    for(auto & c : comps)
    {
        if(c->getPlayed())
            counter++;
        else
            break;
    }
    return counter;
}

QString CompetitionInfo::getShortSerieTypeText()
{
    switch(serieType)
    {
    case Competition:
        return "C";
    case Qualifications:
        return "Q";
    case TrialRound:
        return "Tri";
    case Training:
        return "Trn";
    default:
        return "X";
    }
}

QString CompetitionInfo::getLongSerieTypeText()
{
    switch(serieType)
    {
    case Competition:
        return QObject::tr("Konkurs");
    case Qualifications:
        return QObject::tr("Kwalifikacje");
    case TrialRound:
        return QObject::tr("Seria próbna");
    case Training:
        return QObject::tr("Trening");
    default:
        return QObject::tr("<Błąd!!!>");
    }
}

QString CompetitionInfo::getSingleResultsTextForWebhook(AbstractCompetitionManager * manager)
{
    CompetitionInfo * comp = this;
    CompetitionResults * res = &comp->getResultsReference();

    QString fullstring;
    if(comp->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
    {
        for(auto & sr : res->getResultsReference())
        {
            QString s;
            QString koEmoji;
            if(comp->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
            {
                IndividualCompetitionManager * im = static_cast<IndividualCompetitionManager*>(manager);
                if(manager != nullptr){
                    KORoundManager * km = im->getKOManager();
                    if(km != nullptr)
                    {
                        if(km->getStatusesReference().count() > 0)
                        {
                            if(km->getLuckyLoserReference().contains(sr.getJumper()) == true)
                                koEmoji = "**(LL)** :yellow_circle:";
                            else if(km->getStatusesReference().value(sr.getJumper()) == KORoundManager::Winner)
                                koEmoji = ":green_circle:";
                            else if(km->getStatusesReference().value(sr.getJumper()) == KORoundManager::Loser)
                                koEmoji = ":red_circle:";
                            else
                                koEmoji = ":yellow_circle:";
                            koEmoji += " ";
                        }
                    }
                }
            }
            s += koEmoji + QString::number(sr.getPosition()) + ". " + sr.getJumper()->getTextForDiscord() + ": ";
            int i=0;
            for(auto & jump : sr.getJumpsReference()){
                s += "*" + QString::number(jump.getDistance(), 'f', 1) + "m" + "*";
                i++;
                if(i < sr.getJumpsReference().count())
                    s += " | ";
            }
            s += " --> **" + QString::number(sr.getPointsSum(), 'f', 1) + QObject::tr("pkt") + "**\n";
            fullstring += s;
        }
    }
    else
    {
        for(auto & sr : res->getResultsReference())
        {
            QString ss = "__" + GlobalDatabase::get()->getCountryByAlpha3(sr.getTeam()->getCountryCode())->getName() + QString(" :flag_%1:").arg(GlobalDatabase::get()->getCountryByAlpha3(sr.getTeam()->getCountryCode())->getAlpha2().toLower()) + QString("__** --> %1**\n").arg(QString::number(sr.getPointsSum(), 'f', 1) + QObject::tr("pkt"));
            fullstring += ss;
            for(auto & tjr : sr.getTeamJumpersResultsReference())
            {
                QString s;
                s = tjr.getJumper()->getNameAndSurname() + ": ";
                int i=0;
                for(auto & jump : tjr.getJumpsReference())
                {
                    s += "*" + QString::number(jump.getDistance(), 'f', 1) + "m*";
                    i++;
                    if(i < tjr.getJumpsReference().count())
                        s += " | ";
                }
                fullstring += s + "\n";
            }
        }
    }
    return fullstring;
}

dpp::message CompetitionInfo::getResultsWebhookMessage(AbstractCompetitionManager * manager)
{
    dpp::message message;

    QString typeText;
    QString koRoundText;
    if(getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
        typeText = QObject::tr("(indywidualny)");
    else
        typeText = QObject::tr("(drużynowy)");

    if(rules.getCompetitionType() == CompetitionRules::Individual)
    {
        IndividualCompetitionManager * im = static_cast<IndividualCompetitionManager*>(manager);
        if(manager != nullptr){
            KORoundManager * km = im->getKOManager();
            if(km != nullptr)
            {
                koRoundText = QObject::tr("(runda KO)");
            }
        }
    }

    QString title = "**" + getHill()->getHillTextForDiscord() + " - " + getLongSerieTypeText() + " " + typeText + " " + koRoundText + "**";
    QString description;

    int index=manager->getActualStartListIndex();
    if(manager->isAllJumpsAreFinished())
        index++;
    if(manager != nullptr){
    if(getRulesPointer()->getCompetitionType() == CompetitionRules::Team)
    {
        description = QObject::tr("*Po %1 z %2 skoków grupy %3 (runda %4)*").arg(index).arg(static_cast<TeamCompetitionManager *>(manager)->getActualRoundTeamsReference().count()).arg(static_cast<TeamCompetitionManager *>(manager)->getActualGroup()).arg(manager->getActualRound());
    }
    else
    {
        description = QObject::tr("*Po %1 z %2 skoków rundy %3*").arg(index).arg(static_cast<IndividualCompetitionManager *>(manager)->getActualRoundJumpersReference().count()).arg(manager->getActualRound());
    }
    }
    else
    {
        description = QObject::tr("*Po wszystkich skokach konkursu*");
    }

    message.content = title.toStdString() + "\n";
    message.content += description.toStdString() + "\n";
    message.content += QObject::tr("### Wyniki:").toStdString() + "\n" + getSingleResultsTextForWebhook(manager).toStdString() + "\n";
    message.content += QObject::tr("\n*Wiadomość wysłana z poziomu Sj.Sim").toStdString() + appVersion.toStdString() + "*";

    return message;
}

void CompetitionInfo::sendResultsWebhook(AbstractCompetitionManager * manager)
{
    dpp::message msg = getResultsWebhookMessage(manager);
    std::string content = msg.content;
    int i = 0;
    while (!content.empty())
    {
        dpp::cluster bot("");
        dpp::webhook wh(GlobalAppSettings::get()->getCompetitionResultsWebhook().toStdString());
        std::string newContent = content.substr(0, std::min(2000, int(content.length())));
        content.replace(0, newContent.length(), "");
        bot.execute_webhook(wh, dpp::message(newContent));
        i++;
    }

}

QVector<Jumper *> CompetitionInfo::getStartList() const
{
    return startList;
}

QVector<Jumper *> &CompetitionInfo::getStartListReference()
{
    return startList;
}

void CompetitionInfo::setStartList(const QVector<Jumper *> &newStartList)
{
    startList = newStartList;
}

double CompetitionInfo::getJumpsImportance() const
{
    return jumpsImportance;
}

void CompetitionInfo::setJumpsImportance(double newJumpsImportance)
{
    jumpsImportance = newJumpsImportance;
}

QVector<QVector<KOGroup> > CompetitionInfo::getRoundsKOGroups() const
{
    return roundsKOGroups;
}

void CompetitionInfo::setRoundsKOGroups(const QVector<QVector<KOGroup> > &newRoundsKOGroups)
{
    roundsKOGroups = newRoundsKOGroups;
}

QVector<Team> CompetitionInfo::getTeams() const
{
    return teams;
}

QVector<Team> &CompetitionInfo::getTeamsReference()
{
    return teams;
}

void CompetitionInfo::setTeams(const QVector<Team> &newTeams)
{
    teams = newTeams;
}

QVector<QVector<KOGroup> > &CompetitionInfo::getRoundsKOGroupsReference()
{
    return roundsKOGroups;
}

bool CompetitionInfo::getPlayed() const
{
    return played;
}

void CompetitionInfo::setPlayed(bool newPlayed)
{
    played = newPlayed;
}

QVector<CompetitionInfo *> CompetitionInfo::getQualifyingCompetitions() const
{
    return qualifyingCompetitions;
}

QVector<CompetitionInfo *> &CompetitionInfo::getQualifyingCompetitionsReference()
{
    return qualifyingCompetitions;
}

void CompetitionInfo::setQualifyingCompetitions(const QVector<CompetitionInfo *> &newQualifyingCompetitions)
{
    qualifyingCompetitions = newQualifyingCompetitions;
}

CompetitionInfo *CompetitionInfo::getAdvancementCompetition() const
{
    return advancementCompetition;
}

void CompetitionInfo::setAdvancementCompetition(CompetitionInfo *newAdvancementCompetition)
{
    advancementCompetition = newAdvancementCompetition;
}

Classification *CompetitionInfo::getAdvancementClassification() const
{
    return advancementClassification;
}

void CompetitionInfo::setAdvancementClassification(Classification *newAdvancementClassification)
{
    advancementClassification = newAdvancementClassification;
}

CompetitionResults CompetitionInfo::getResults() const
{
    return results;
}

void CompetitionInfo::setResults(const CompetitionResults &newResults)
{
    results = newResults;
}

CompetitionResults &CompetitionInfo::getResultsReference()
{
    return results;
}

QJsonObject CompetitionInfo::getJsonObject(CompetitionInfo &competition)
{
    QJsonObject object;
    object.insert("id", competition.getIDStr());
    if(competition.getHill() != nullptr)
        object.insert("hill-id", competition.getHill()->getIDStr());
    object.insert("results", CompetitionResults::getJsonObject(competition.getResultsReference()));
    object.insert("rules", CompetitionRules::getJsonObject(competition.getRules()));
    object.insert("serie-type", competition.getSerieType());
    object.insert("exceptional-rounds-count", competition.getExceptionalRoundsCount());
    object.insert("cancelled", competition.getCancelled());
    object.insert("played", competition.getPlayed());
    object.insert("jumps-importance", competition.getJumpsImportance());

    if(competition.getTrialRound() != nullptr)
        object.insert("trial-round-id", competition.getTrialRound()->getIDStr());
    QJsonArray trainingsArray;
    for(auto & t : qAsConst(competition.getTrainingsReference())){
        trainingsArray.push_back(t->getIDStr());
    }
    object.insert("training-ids", trainingsArray);

    QJsonArray classificationsArray;
    for(auto & c : qAsConst(competition.getClassificationsReference())){
        if(c != nullptr)
        classificationsArray.push_back(c->getIDStr());
    }
    object.insert("classifications-ids", classificationsArray);

    if(competition.getAdvancementClassification() != nullptr)
        object.insert("advancement-classification-id", competition.getAdvancementClassification()->getIDStr());
    if(competition.getAdvancementCompetition() != nullptr)
        object.insert("advancement-competition-id", competition.getAdvancementCompetition()->getIDStr());

    QJsonArray teamsArray;
    for(auto & team : competition.getTeamsReference())
    {
        teamsArray.push_back(Team::getJsonObject(team));
    }
    object.insert("teams", teamsArray);

    QJsonArray KOGroupsArray;
    for(auto & groups : competition.getRoundsKOGroupsReference())
    {
        QJsonArray groupArray;
        for(auto & group : groups)
            groupArray.push_back(KOGroup::getJsonObject(group));
        KOGroupsArray.push_back(groupArray);
    }
    object.insert("rounds-ko-groups", KOGroupsArray);

    QJsonArray startListArray;
    //qDebug()<<"slc "<<competition.getStartListReference().count();
    for(auto & j : competition.getStartList())
    {
        startListArray.push_back(j->getIDStr());
    }
    object.insert("start-list", startListArray);

    return object;
}

CompetitionInfo CompetitionInfo::getFromJson(const QJsonObject &json, IdentifiableObjectsStorage * storage, int * oldId, QMap<ulong, Identifiable*> * before120Map)
{
    CompetitionInfo comp;
    QJsonArray teamsArray = json.value("teams").toArray();
    for(auto val : teamsArray)
    {
        comp.getTeamsReference().push_back(Team::getFromJson(val.toObject(), storage));
    }
    storage->add(comp.getTeamsReference());

    QJsonArray KOGroupsArray = json.value("rounds-ko-groups").toArray();
    for(auto val : KOGroupsArray)
    {
        QVector<KOGroup> groups;
        for(auto v : val.toArray())
        {
            groups.push_back(KOGroup::getFromJson(v.toObject(), storage));
        }
        comp.getRoundsKOGroupsReference().push_back(groups);
        storage->add(groups);
    }
    //seasonObjectsManager.fill(&comp.getKOGroupsReference());

    comp.setID(sole::rebuild(json.value("id").toString().toStdString()));
    if(oldId)
    {
        *oldId = json.value("id").toString().toULong();
    }
    if(!before120Map)
        comp.setHill(static_cast<Hill *>(storage->get(json.value("hill-id").toString())));
    comp.setResults(CompetitionResults::getFromJson(json.value("results").toObject(), storage));
    if(storage != nullptr)
        storage->add(&comp.getResultsReference());
    for(auto & sr : comp.getResultsReference().getResultsReference()){
        sr.setCompetition(&comp);
        for(auto & j : sr.getJumpsReference()){
            j.setCompetition(sr.getCompetition());
            j.setSingleResult(&sr);
        }
    }
    comp.getResultsReference().setCompetition(&comp);
    comp.setRules(CompetitionRules::getFromJson(json.value("rules").toObject()));
    comp.setSerieType(json.value("serie-type").toInt());
    comp.setExceptionalRoundsCount(json.value("exceptional-rounds-count").toInt());
    comp.setCancelled(json.value("cancelled").toBool());
    comp.setPlayed(json.value("played").toBool());
    comp.setJumpsImportance(json.value("jumps-importance").toDouble(5));

    if(before120Map)
        comp.setTrialRound(static_cast<CompetitionInfo *>(before120Map->value(json.value("trial-round-id").toString().toULong())));
    else
        comp.setTrialRound(static_cast<CompetitionInfo *>(storage->get(json.value("trial-round-id").toString())));

    QJsonArray trainingsArray = json.value("training-ids").toArray();
    for(auto val : trainingsArray){
        if(before120Map)
            comp.getTrainingsReference().push_back(static_cast<CompetitionInfo *>(before120Map->value(val.toString().toULong())));
        else
            comp.getTrainingsReference().push_back(static_cast<CompetitionInfo *>(storage->get(val.toString())));
    }

    QJsonArray classificationsArray = json.value("classifications-ids").toArray();
    for(auto val : classificationsArray){
        if(before120Map)
            comp.getClassificationsReference().push_back(static_cast<Classification *>(before120Map->value(val.toString().toULong())));
        else
            comp.getClassificationsReference().push_back(static_cast<Classification *>(storage->get(val.toString())));
    }

    QJsonArray startListArray = json.value("start-list").toArray();
    for(auto val : startListArray)
    {
        comp.getStartListReference().push_back(static_cast<Jumper *>(storage->get(val.toString())));
    }

    QString advClsVal = json.value("advancement-classification-id").toString();
    if(!advClsVal.isEmpty())
    {
    if(before120Map)
        comp.setAdvancementClassification(static_cast<Classification *>(before120Map->value(advClsVal.toULong())));
    else
        comp.setAdvancementClassification(static_cast<Classification *>(storage->get(advClsVal)));
    }

QString advCompVal = json.value("advancement-competition-id").toString();
    if(advCompVal.isEmpty() == false){
    if(before120Map)
        comp.setAdvancementCompetition(static_cast<CompetitionInfo *>(before120Map->value(advCompVal.toULong())));
    else
        comp.setAdvancementCompetition(static_cast<CompetitionInfo *>(storage->get(advCompVal)));
    }

    return comp;
}

QVector<Classification *> CompetitionInfo::getClassifications() const
{
    return classifications;
}

QVector<Classification *> &CompetitionInfo::getClassificationsReference()
{
    return classifications;
}

void CompetitionInfo::setClassifications(const QVector<Classification *> &newClassifications)
{
    classifications = newClassifications;
}

QVector<CompetitionInfo *> CompetitionInfo::getTrainings() const
{
    return trainings;
}

QVector<CompetitionInfo *> &CompetitionInfo::getTrainingsReference()
{
    return trainings;
}

void CompetitionInfo::setTrainings(const QVector<CompetitionInfo *> &newTrainings)
{
    trainings = newTrainings;
}

CompetitionInfo *CompetitionInfo::getTrialRound() const
{
    return trialRound;
}

void CompetitionInfo::setTrialRound(CompetitionInfo *newTrialRound)
{
    trialRound = newTrialRound;
}

bool CompetitionInfo::getCancelled() const
{
    return cancelled;
}

void CompetitionInfo::setCancelled(bool newCancelled)
{
    cancelled = newCancelled;
}

int CompetitionInfo::getExceptionalRoundsCount() const
{
    return exceptionalRoundsCount;
}

void CompetitionInfo::setExceptionalRoundsCount(int newExceptionalRoundsCount)
{
    exceptionalRoundsCount = newExceptionalRoundsCount;
}

short CompetitionInfo::getSerieType() const
{
    return serieType;
}

void CompetitionInfo::setSerieType(short newSerieType)
{
    serieType = newSerieType;
}

Hill *CompetitionInfo::getHill() const
{
    return hill;
}

void CompetitionInfo::setHill(Hill *newHill)
{
    hill = newHill;
}

CompetitionRules CompetitionInfo::getRules() const
{
    return rules;
}

void CompetitionInfo::setRules(const CompetitionRules &newRules)
{
    rules = newRules;
}
