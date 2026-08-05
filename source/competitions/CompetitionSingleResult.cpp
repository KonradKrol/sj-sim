#include "CompetitionSingleResult.h"
#include "../global/IdentifiableObjectsStorage.h"
#include "../global/GlobalDatabase.h"
#include "CompetitionInfo.h"

extern const QString appVersion;

CompetitionSingleResult::CompetitionSingleResult(CompetitionInfo *competition, Jumper *jumper, int type) : type(type), jumper(jumper), team(nullptr), competition(competition),
    Identifiable()
{}

CompetitionSingleResult::CompetitionSingleResult(CompetitionInfo *competition, Team *team, int type) : type(type), jumper(nullptr), team(team), competition(competition),
    Identifiable()
{}

CompetitionSingleResult CompetitionSingleResult::getFromJson(QJsonObject obj, IdentifiableObjectsStorage * storage)
{
    CompetitionSingleResult result(nullptr);
    result.setID(sole::rebuild(obj.value("id").toString().toStdString()));
    //result.setCompetition(static_cast<CompetitionInfo *>(storage->get(obj.value("competition-id").toString())));
    result.setPosition(obj.value("position").toInt());
    result.setPointsSum(obj.value("points-sum").toDouble());
    result.setType(obj.value("type").toInt());
    result.setJumper(static_cast<Jumper *>(storage->get(obj.value("jumper-id").toString())));
    result.setTeam(static_cast<Team *>(storage->get(obj.value("team-id").toString())));

    QJsonArray jumpsArray = obj.value("jumps").toArray();
    for(auto jump : jumpsArray){
        result.getJumpsReference().push_back(JumpData::getFromJson(jump.toObject(), storage));
    }
    result.updateTeamJumpersResults();
    return result;
}

QJsonObject CompetitionSingleResult::getJsonObject(CompetitionSingleResult result)
{
    QJsonObject resultObject;
    resultObject.insert("id", result.getIDStr());
    //resultObject.insert("competition-id", QString::number(result.getCompetition()->getID()));
    resultObject.insert("position", result.getPosition());
    resultObject.insert("points-sum", result.getPointsSum());
    resultObject.insert("type", result.getType());
    if(result.getJumper() != nullptr)
        resultObject.insert("jumper-id", result.getJumper()->getIDStr());
    if(result.getTeam() != nullptr)
        resultObject.insert("team-id", result.getTeam()->getIDStr());

    QJsonArray jumpsArray;
    for(auto & jump : qAsConst(result.getJumpsReference())){
        QJsonObject jumpObject = JumpData::getJsonObject(jump);
        jumpsArray.push_back(jumpObject);
    }
    resultObject.insert("jumps", jumpsArray);

    return resultObject;
}

CompetitionSingleResult CompetitionSingleResult::getFromJsonValue(QJsonValue val, IdentifiableObjectsStorage *storage)
{
    return CompetitionSingleResult::getFromJson(val.toObject(), storage);
}

QVector<CompetitionSingleResult *> CompetitionSingleResult::getFilteredSingleResults(QVector<CompetitionInfo *> &competitions, Jumper *jumper, QSet<int> serieTypes, QSet<int> hillTypes, QVector<Classification *> classifications, bool skipClassifications, Hill * specificHill)
{
    QVector<CompetitionSingleResult *> singleResults;
    for(auto & comp : competitions)
    {
        bool ok = false;
        for(auto & compCls : comp->getClassificationsReference()){
            for(auto & cls : classifications){
                if(compCls->getName() == cls->getName())
                {
                    ok = true;
                    break;
                }
            }
        }
        if(skipClassifications == true)
            ok = true;

        bool hillOk = true;
        if(specificHill == nullptr)
            hillOk = true;
        else if(comp->getHill() != specificHill)
            hillOk = false;

        if(serieTypes.contains(comp->getSerieType()) && hillTypes.contains(comp->getHill()->getHillType()) && ok && hillOk)
        {
            if(comp->getResultsReference().getResultOfIndividualJumper(jumper) != nullptr)
            {
                singleResults.push_back(comp->getResultsReference().getResultOfIndividualJumper(jumper));
                continue;
            }
        }
    }
    return singleResults;
}

QHash<Jumper *, QVector<CompetitionSingleResult *> > CompetitionSingleResult::getJumpersFilteredSingleResults(QVector<Jumper *> &jumpers, QVector<CompetitionInfo *> &competitions, QSet<int> serieTypes,QSet<int> hillTypes,  QVector<Classification *> classifications, bool skipClassifications, Hill * specificHill)
{
    QHash<Jumper *, QVector<CompetitionSingleResult *>> toReturn;
    for(auto & jumper : jumpers)
    {
        toReturn.insert(jumper, CompetitionSingleResult::getFilteredSingleResults(competitions, jumper, serieTypes, hillTypes, classifications, skipClassifications, specificHill));
    }
    return toReturn;
}

QHash<QString, QVector<CompetitionSingleResult *> > CompetitionSingleResult::getTeamsFilteredSingleResults(QVector<QString> &teams, QVector<CompetitionInfo *> &competitions, QSet<int> serieTypes, QSet<int> hillTypes, QVector<Classification *> classifications, bool skipClassifications, Hill *specificHill)
{
    QHash<QString, QVector<CompetitionSingleResult *>> toReturn;
    for(auto & team : teams)
    {
        toReturn.insert(team, CompetitionSingleResult::getFilteredSingleResultsForTeam(competitions, team, serieTypes, hillTypes, classifications, skipClassifications, specificHill));
    }
    return toReturn;
}

QVector<CompetitionSingleResult *> CompetitionSingleResult::getFilteredSingleResultsForTeam(QVector<CompetitionInfo *> &competitions, QString countryCode, QSet<int> serieTypes, QSet<int> hillTypes, QVector<Classification *> classifications, bool skipClassifications, Hill *specificHill)
{
    QVector<CompetitionSingleResult *> singleResults;
    for(auto & comp : competitions)
    {
        if(comp->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
            continue;
        bool ok = false;
        for(auto & compCls : comp->getClassificationsReference()){
            for(auto & cls : classifications){
                if(compCls->getName() == cls->getName())
                {
                    ok = true;
                    break;
                }
            }
        }
        if(skipClassifications == true)
            ok = true;

        bool hillOk = true;
        if(specificHill == nullptr)
            hillOk = true;
        else if(comp->getHill() != specificHill)
            hillOk = false;

        if(serieTypes.contains(comp->getSerieType()) && hillTypes.contains(comp->getHill()->getHillType()) && ok && hillOk)
        {
            if(comp->getResultsReference().getResultOfTeam(Team::getTeamByCountryCode(&comp->getTeamsReference(), countryCode)) != nullptr)
            {
                singleResults.push_back(comp->getResultsReference().getResultOfTeam(Team::getTeamByCountryCode(&comp->getTeamsReference(), countryCode)));
                continue;
            }
        }
    }
    return singleResults;
}

QString CompetitionSingleResult::getCsvResultsObject()
{
    QString s;
    s += QString::number(position)+";";
    if(jumper != nullptr){
    s += jumper->getNameAndSurname()+";";
    s += jumper->getCountryCode()+";";
    }
    else
    {
    s += GlobalDatabase::get()->getCountryByAlpha3(team->getCountryCode())->getName()+";";
    s += team->getCountryCode()+";";
    }
    s += QString::number(pointsSum)+";";
    for(auto & jump : jumps)
    {
        s += QString::number(jump.getDistance())+";";
        s += QString::number(jump.getGate())+";";
        s += QString::number(jump.getAveragedWind())+";";
        s += QString::number(jump.getTotalCompensation())+";";
        s += QString::number(jump.getJudgesPoints())+";";
        s += QString::number(jump.getPoints())+";";
        s.replace(".", ",");
    }
    return s;
}

QString CompetitionSingleResult::getIndividualJumpsSummaryText()
{
    QString s;
    int round=1;
    for(auto & jump : jumps)
    {
        QString jumpInfo = QString::number(jump.getDistance(), 'f', 1) + "m - " +  QString::number(jump.getPoints(), 'f', 1) + QObject::tr("pkt") + QObject::tr(" (%1 miejsce w tej serii)").arg(jump.getPositionInRound()) + "\n";
        s += QString(QObject::tr("Runda %1: %2")).arg(round).arg(jumpInfo);
        round++;
    }
    return s;
}

QString CompetitionSingleResult::getTeamJumpsSummaryText()
{
    QString s = QObject::tr("__Runda %1\n__").arg(1);
    int round=1;
    int remaining = team->getJumpersCount();
    for(auto & jump : jumps)
    {
        s += jump.getJumper()->getNameAndSurname() + ": " + QString::number(jump.getDistance(), 'f', 1) + "m - " +  QString::number(jump.getPoints(), 'f', 1) + QObject::tr("pkt") + "\n";
        remaining--;
        if(remaining == 0)
        {
            remaining = team->getJumpersCount();
            round++;
            if(round <= competition->getRulesPointer()->getRoundsReference().count())
                s += QObject::tr("__Runda %1__").arg(round) + "\n";
        }
    }
    return s;
}

dpp::embed CompetitionSingleResult::getEmbedForIndividualSingleCompResult()
{
    dpp::embed embed;
    embed.set_color(dpp::colors::coffee);
    QString title = QString("**%1 %2**").arg(jumper->getNameAndSurname()).arg(QString(":flag_%1:").arg(GlobalDatabase::get()->getCountryByAlpha3(jumper->getCountryCode())->getAlpha2().toLower()));
    embed.set_title(title.toStdString());
    QString description = competition->getHill()->getHillTextForDiscord() + QString(" (%1)").arg(competition->getLongSerieTypeText()) + " (indywidualny)";
    embed.set_description(description.toStdString());

    embed.add_field(QObject::tr("Pozycja").toStdString(), QString::number(position).toStdString(), true);
    embed.add_field(QObject::tr("Nota łączna").toStdString(), QString::number(pointsSum, 'f', 1).toStdString(), true);
    embed.add_field(QObject::tr("Skoki").toStdString(), getIndividualJumpsSummaryText().toStdString(), false);

    embed.set_footer(
        dpp::embed_footer()
            .set_text(QObject::tr("Wiadomość wysłana z poziomu Sj.Sim ").toStdString() + appVersion.toStdString() + "\n" + "https://github.com/SiekamCebule/sj-sim")
        );

    return embed;
}

dpp::embed CompetitionSingleResult::getEmbedForTeamSingleCompResult()
{
    Country * country = GlobalDatabase::get()->getCountryByAlpha3(team->getCountryCode());
    dpp::embed embed;
    embed.set_color(dpp::colors::coffee);
    QString title = QString("**%1 %2**").arg(country->getName()).arg(QString(":flag_%1:").arg(country->getAlpha2().toLower()));
    embed.set_title(title.toStdString());
    QString description = competition->getHill()->getHillTextForDiscord() + QString(" (%1)").arg(competition->getLongSerieTypeText()) + " (drużynowy)";
    embed.set_description(description.toStdString());

    embed.add_field(QObject::tr("Pozycja").toStdString(), QString::number(position).toStdString(), true);
    embed.add_field(QObject::tr("Nota łączna").toStdString(), QString::number(pointsSum, 'f', 1).toStdString(), true);
    embed.add_field(QObject::tr("Skoki").toStdString(), getTeamJumpsSummaryText().toStdString(), false);

    embed.set_footer(
        dpp::embed_footer()
            .set_text(QObject::tr("Wiadomość wysłana z poziomu Sj.Sim ").toStdString() + appVersion.toStdString() + "\n" + "https://github.com/SiekamCebule/sj-sim")
        );

    return embed;
}

CompetitionInfo *CompetitionSingleResult::getCompetition() const
{
    return competition;
}

void CompetitionSingleResult::setCompetition(CompetitionInfo *newCompetition)
{
    competition = newCompetition;
}

QVector<CompetitionSingleResult> CompetitionSingleResult::getTeamJumpersResults() const
{
    return teamJumpersResults;
}

QVector<CompetitionSingleResult> &CompetitionSingleResult::getTeamJumpersResultsReference()
{
    return teamJumpersResults;
}

void CompetitionSingleResult::setTeamJumpersResults(const QVector<CompetitionSingleResult> &newTeamJumpersResults)
{
    teamJumpersResults = newTeamJumpersResults;
}

int CompetitionSingleResult::getPosition() const
{
    return position;
}

void CompetitionSingleResult::setPosition(int newPosition)
{
    position = newPosition;
}

Team *CompetitionSingleResult::getTeam() const
{
    return team;
}

void CompetitionSingleResult::setTeam(Team *newTeam)
{
    team = newTeam;
}

Jumper *CompetitionSingleResult::getJumper() const
{
    return jumper;
}

void CompetitionSingleResult::setJumper(Jumper *newJumper)
{
    jumper = newJumper;
}

int CompetitionSingleResult::getType() const
{
    return type;
}

void CompetitionSingleResult::setType(int newType)
{
    type = newType;
}

double CompetitionSingleResult::getPointsSum() const
{
    return pointsSum;
}

void CompetitionSingleResult::setPointsSum(double newPointsSum)
{
    pointsSum = newPointsSum;
}

QVector<JumpData> CompetitionSingleResult::getJumps() const
{
    return jumps;
}

QVector<JumpData> &CompetitionSingleResult::getJumpsReference()
{
    return jumps;
}

void CompetitionSingleResult::setJumps(const QVector<JumpData> &newJumps)
{
    jumps = newJumps;
}

void CompetitionSingleResult::updatePointsSum()
{
    pointsSum = 0;
    for(auto & res : jumps){
        pointsSum += res.getPoints();
    }
}

void CompetitionSingleResult::updateTeamJumpersResults()
{
    teamJumpersResults.clear();
    QSet<Jumper *> jumpers;
    for(auto & jump : getJumpsReference()){
        if(jumpers.contains(jump.getJumper()) == false){
            CompetitionSingleResult result(competition, jump.getJumper());
            result.getJumpsReference().push_back(jump);
            result.updatePointsSum();

            teamJumpersResults.push_back(result);
            jumpers.insert(jump.getJumper());
        }
        else{
            for(auto & result : teamJumpersResults){
                if(jump.getJumper() == result.getJumper()){
                    result.getJumpsReference().push_back(jump);
                    result.updatePointsSum();
                }
            }
        }
    }
}

CompetitionSingleResult *CompetitionSingleResult::getTeamJumperResult(Jumper *jumper)
{
    for(auto & res : teamJumpersResults)
    {
        if(res.getJumper() == jumper)
            return &res;
    }
}
