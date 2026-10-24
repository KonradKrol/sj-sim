#include "CompetitionSingleResult.h"
#include "../global/IdentifiableObjectsStorage.h"
#include "../global/GlobalDatabase.h"
#include "CompetitionInfo.h"
#include <algorithm>

extern const QString appVersion;

CompetitionSingleResult::CompetitionSingleResult(CompetitionInfo *competition, Jumper *jumper, int type) : type(type), jumper(jumper), team(nullptr), competition(competition),
    pointsSum(0), position(0),
    Identifiable()
{}

CompetitionSingleResult::CompetitionSingleResult(CompetitionInfo *competition, Team *team, int type) : type(type), jumper(nullptr), team(team), competition(competition),
    pointsSum(0), position(0),
    Identifiable()
{}

CompetitionSingleResult CompetitionSingleResult::getFromJson(QJsonObject obj, IdentifiableObjectsStorage * storage)
{
    CompetitionSingleResult result(nullptr);
    result.setID(sole::rebuild(obj.value("id").toString().toStdString()));
    //result.setCompetition(static_cast<CompetitionInfo *>(storage->get(obj.value("competition-id").toString())));
    result.setPosition(obj.value("position").toInt());
    result.setPointsSum(obj.value("points-sum").toDouble());
    result.scoringRound = qMax(0,obj.value("scoring-round").toInt());
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
    resultObject.insert("scoring-round", result.getScoringRound());
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
    const auto textField = [](QString value) {
        if(value.contains(';') || value.contains('"') || value.contains('\n') || value.contains('\r')) {
            value.replace("\"", "\"\"");
            return "\"" + value + "\"";
        }
        return value;
    };
    const auto numberField = [](double value) { return QString::number(value).replace('.', ','); };
    QStringList fields{QString::number(position)};
    if(jumper != nullptr) {
        fields << textField(jumper->getNameAndSurname()) << textField(jumper->getCountryCode());
    } else if(team != nullptr) {
        QString name = team->getCountryCode();
        for(const auto &country : GlobalDatabase::get()->getCountries())
            if(country.getAlpha3() == team->getCountryCode() && !country.getName().isEmpty()) { name = country.getName(); break; }
        fields << textField(name) << textField(team->getCountryCode());
    } else {
        fields << QString() << QString();
    }
    fields << numberField(pointsSum);
    for(const auto &jump : jumps)
        fields << numberField(jump.getDistance()) << QString::number(jump.getGate())
               << numberField(jump.getAveragedWind()) << numberField(jump.getTotalCompensation())
               << numberField(jump.getJudgesPoints()) << numberField(jump.getPoints());
    return fields.join(';') + ';';
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
    for(auto &result : teamJumpersResults) {
        result.setCompetition(newCompetition);
        result.updatePointsSum(getScoringRound());
    }
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

int CompetitionSingleResult::getScoringRound() const {
    if(!competition && scoringRound>0)return scoringRound;
    const int perRound=type==TeamResult && competition?qMax(1,competition->getRulesPointer()->getJumpersInTeamCount()):1;
    return qMax(scoringRound,int((jumps.size()+perRound-1)/perRound));
}

bool CompetitionSingleResult::isTeamDisqualified() const {
    return type==TeamResult && std::any_of(jumps.cbegin(),jumps.cend(),[](const JumpData &jump){return jump.getDSQ();});
}

int CompetitionSingleResult::getRankingRound() const {
    if(isTeamDisqualified())return -1;
    return competition && competition->getRulesPointer()->hasRoundPointAdjustments()?getScoringRound():0;
}

qint64 CompetitionSingleResult::getRankingPoints() const {
    return qRound64(pointsSum * 10.);
}

bool CompetitionSingleResult::hasSameRankAs(const CompetitionSingleResult &other) const {
    return getRankingRound() == other.getRankingRound() && getRankingPoints() == other.getRankingPoints();
}

double CompetitionSingleResult::pointsForJumps(int jumpCount,int round) const {
    jumpCount=qBound(0,jumpCount,int(jumps.size()));
    const auto *rules=competition?competition->getRulesPointer():nullptr;
    const int perRound=type==TeamResult && rules?qMax(1,rules->getJumpersInTeamCount()):1;
    if(round<=0)round=(jumpCount+perRound-1)/perRound;
    const bool adjusted=rules && rules->hasRoundPointAdjustments();
    const auto weights=adjusted?rules->getRoundPointWeights(round):QVector<double>();
    double total=0;
    for(int index=0;index<jumpCount;++index)total+=jumps[index].getPoints()*weights.value(index/perRound,1.);
    return adjusted?qRound64(total*10)/10.:total;
}

void CompetitionSingleResult::updatePointsSum(int round) {
    scoringRound=round>=0?round:getScoringRound();
    pointsSum=pointsForJumps(jumps.size(),getScoringRound());
}

void CompetitionSingleResult::updateTeamJumpersResults()
{
    teamJumpersResults.clear();
    QSet<Jumper *> jumpers;
    for(auto & jump : getJumpsReference()){
        if(jumpers.contains(jump.getJumper()) == false){
            CompetitionSingleResult result(competition, jump.getJumper());
            result.scoringRound=getScoringRound();
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
    return nullptr;
}
