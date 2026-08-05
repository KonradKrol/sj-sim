#include "CompetitionResults.h"

#include "CompetitionInfo.h"
#include "CompetitionSingleResult.h"
#include <QtConcurrent>

extern Uuid globalIDGenerator;

CompetitionResults::CompetitionResults() :
    Identifiable()
{

}

CompetitionInfo *CompetitionResults::getCompetition() const
{
    return competition;
}

void CompetitionResults::setCompetition(CompetitionInfo *newCompetition)
{
    competition = newCompetition;
}

QVector<QVector<JumpData *> > CompetitionResults::constructRoundsJumps(QVector<RoundInfo> *rounds)
{
    QVector<QVector<JumpData *>> roundsJumps;
    for(int i=0; i<rounds->count(); i++){
        QVector<JumpData *> jumps;
        for(auto & jp : results){
            if(jp.getJumpsReference().count() < i){
                jumps.push_back(&jp.getJumpsReference()[i]);
            }
        }
        roundsJumps.push_back(jumps);
    }
    return roundsJumps;
}

CompetitionResults CompetitionResults::constructRoundsResults(QVector<RoundInfo> *roundsInfos, QVector<int> rounds)
{
    CompetitionResults results;
    for(auto & round : rounds){
        if(round - 1 >= results.getResultsReference().count())
            break;
        else{
            for(auto & jump : constructRoundsJumps(roundsInfos)[round - 1]){
                results.addJump(jump->getJumper(), *jump);
            }
        }
    }
    results.sortInDescendingOrder();
    return results;
}

CompetitionResults CompetitionResults::getFromJson(QJsonObject obj, IdentifiableObjectsStorage * storage)
{
    CompetitionResults results;
    results.setID(sole::rebuild(obj.value("id").toString().toStdString()));

    QJsonArray resultsArray = obj.value("results").toArray();
    QVector<QJsonValue> values;
    for(auto val : resultsArray)
        values.push_back(val);

    QVector<CompetitionSingleResult> parsedResults;
    parsedResults.reserve(values.size());
    for (const auto &value : values)
        parsedResults.append(CompetitionSingleResult::getFromJson(value.toObject(), storage));
    results.setResults(parsedResults);

    storage->add(results.getResultsReference());

    return results;
}

QJsonObject CompetitionResults::getJsonObject(CompetitionResults &results)
{
    QJsonObject object;
    object.insert("id", results.getIDStr());

    QJsonArray resultsArray;
    for (const auto &result : results.getResultsReference())
        resultsArray.append(CompetitionSingleResult::getJsonObject(result));
    object.insert("results", resultsArray);

    //qDebug()<<QString("full res of comp -> end (%1) (%2)").arg(resultsArray.count()).arg(results.getCompetition()->getHill()->getHillText());
    return object;
}

QVector<int> CompetitionResults::getJumpersPositions(const QVector<Jumper *> * jumpers) const
{
    QVector<int> positions;

    double previousResultPoints = 0;
    int actualPosition = 1;
    int add = 1;
    int i=0;
    for(auto & result : results){
        if(jumpers->contains(result.getJumper())){
            int pos = 0;
            if(i==0){
                positions.push_back(1);
                previousResultPoints = result.getPointsSum();
                i++;
                continue;
            }

            if(previousResultPoints == result.getPointsSum())
            {
                pos = actualPosition;
                add += 1;
            }
            else{
                actualPosition += add;
                add = 1;
                pos = actualPosition;
            }

            previousResultPoints = result.getPointsSum();
            i++;
            positions.push_back(pos);
        }
    }

    return positions;
}

QHash<Jumper *, int> CompetitionResults::getHashWithJumpersPositions(const QVector<Jumper *> * jumpers, int round) const
{
    QHash<Jumper *, int> hash;
    CompetitionResults results = *this;
    results.getResultsReference().detach();
    for(auto & res : results.getResultsReference())
    {
        if(round > (-1))
        {
            if(res.getJumpsReference().count() > round)
                res.getJumpsReference().remove(round, res.getJumpsReference().count() - round);
            res.updateTeamJumpersResults();
            res.updatePointsSum();
        }
    }
    results.sortInDescendingOrder();
    /*
    * Skok 1
    * Skok 2
    * Skok 3
    * Skok 4

    * Runda 2 czyli usuwamy index 2 i 3
    */

    double previousResultPoints = 0;
    int actualPosition = 1;
    int add = 1;
    int i=0;
    for(auto & result : results.getResultsReference()){
        if(jumpers->contains(result.getJumper())){
            int pos = 0;
            if(i==0){
                hash.insert(result.getJumper(), 1);
                previousResultPoints = result.getPointsSum();
                i++;
                continue;
            }

            if(previousResultPoints == result.getPointsSum())
            {
                pos = actualPosition;
                add += 1;
            }
            else{
                actualPosition += add;
                add = 1;
                pos = actualPosition;
            }

            previousResultPoints = result.getPointsSum();
            i++;
            hash.insert(result.getJumper(), pos);
        }
    }

    return hash;
}

void CompetitionResults::sortJumpersByResults(QVector<Jumper *> &jumpers)
{
    QVector<Jumper *> temp;
    for(auto & res : results)
    {
        if(jumpers.contains(res.getJumper()))
        {
            temp.push_back(res.getJumper());
        }
    }
    jumpers = temp;
}

QHash<Jumper *, int> CompetitionResults::getResultsWithJumpersPositionsForClassificationArchiveResults(QHash<Jumper *, double> results)
{
    QList<QPair<Jumper *, double>> list;
    for (auto it = results.begin(); it != results.end(); ++it)
    {
        list.append(qMakePair(it.key(), it.value()));
    }

    // Sortowanie listy według wartości za pomocą funkcji lambda
    std::sort(list.begin(), list.end(), [] (const QPair<Jumper *, double> &a, const QPair<Jumper *, double> &b) {
        return a.second > b.second;
    });

    QVector<int> positions;
    double previousResultPoints = 0;
    int actualPosition = 1;
    int add = 1;
    int i=0;
    for(auto & pair : list)
    {
        int pos = 0;
        if(i == 0)
        {
            positions.push_back(1);
            previousResultPoints = pair.second;
            i++;
            continue;
        }
        if(previousResultPoints == pair.second)
        {
            pos = actualPosition;
            add = 1;
        }
        else{
            actualPosition += add;
            add = 1;
            pos = actualPosition;
        }

        previousResultPoints = pair.second;
        i++;
        positions.push_back(pos);
    }

    QHash<Jumper *, int> toReturn;
    int ii=0;
    for(auto & pos : positions)
    {
        toReturn.insert(list[ii].first, pos);
        ii++;
    }
    return toReturn;
}

QHash<QString, int> CompetitionResults::getResultsWithTeamsPositionsForClassificationArchiveResults(QHash<QString, double> results)
{
    QList<QPair<QString, double>> list;
    for (auto it = results.begin(); it != results.end(); ++it)
    {
        list.append(qMakePair(it.key(), it.value()));
    }

    // Sortowanie listy według wartości za pomocą funkcji lambda
    std::sort(list.begin(), list.end(), [] (const QPair<QString, double> &a, const QPair<QString, double> &b) {
        return a.second > b.second;
    });

    QVector<int> positions;
    double previousResultPoints = 0;
    int actualPosition = 1;
    int add = 1;
    int i=0;
    for(auto & pair : list)
    {
        int pos = 0;
        if(i == 0)
        {
            positions.push_back(1);
            previousResultPoints = pair.second;
            i++;
            continue;
        }
        if(previousResultPoints == pair.second)
        {
            pos = actualPosition;
            add == 1;
        }
        else{
            actualPosition += add;
            add = 1;
            pos = actualPosition;
        }

        previousResultPoints = pair.second;
        i++;
        positions.push_back(pos);
    }

    QHash<QString, int> toReturn;
    int ii=0;
    for(auto & pos : positions)
    {
        toReturn.insert(list[ii].first, pos);
        ii++;
    }
    return toReturn;
}

QVector<QString> CompetitionResults::getTeamsCodesByIndividualResults(CompetitionResults *results)
{
    QVector<QString> codes;
    for(auto & res : results->getResultsReference())
    {
        if(codes.contains(res.getJumper()->getCountryCode()) == false)
            codes.push_back(res.getJumper()->getCountryCode());
    }
    return codes;
}

QVector<Jumper *> CompetitionResults::getJumpersByTeamResults(CompetitionResults *results)
{
    QVector<Jumper *> jumpers;
    for(auto & res : results->getResultsReference())
    {
        for(auto & jumperResult : res.getTeamJumpersResultsReference()){
            if(jumpers.contains(jumperResult.getJumper()) == false)
                jumpers.push_back(jumperResult.getJumper());
        }
    }
    return jumpers;
}

void CompetitionResults::fixSingleResultsJumps()
{
    for(auto & result : this->results)
    {
        for(auto & jump : result.getJumpsReference())
        {
            jump.setSingleResult(&result);
        }
    }
}

QVector<CompetitionSingleResult> CompetitionResults::getResults() const
{
    return results;
}

QVector<CompetitionSingleResult> &CompetitionResults::getResultsReference()
{
    return results;
}

void CompetitionResults::setResults(const QVector<CompetitionSingleResult> &newResults)
{
    results = newResults;
}

CompetitionSingleResult *CompetitionResults::getResultOfIndividualJumper(Jumper *jumper)
{
    for(auto & result : results)
    {
        if(result.getJumper() == jumper){
            return &result;
        }
    }
    return nullptr;
}

CompetitionSingleResult *CompetitionResults::getResultOfTeam(Team *team)
{
    for(auto & result : results)
    {
        if(result.getTeam() == team)
            return &result;
    }
    return nullptr;
}

CompetitionSingleResult *CompetitionResults::getResultByIndex(int index)
{
    if(results.count() > index){
        return &results[index];
    }
    return nullptr;
}

void CompetitionResults::addJump(Jumper *jumper, JumpData &jump, int jumpNumber)
{
    CompetitionSingleResult *result = nullptr;
    for(auto & res : results){
        if(res.getJumper() == jumper){
            result = &res;
            break;
        }
    }
    if(result == nullptr){
        results.push_back(CompetitionSingleResult(competition, jumper, CompetitionSingleResult::IndividualResult));
        jump.setSingleResult(&results.last());
        jump.setCompetition(results.last().getCompetition());
        result = &results.last();
    }
    int index = jumpNumber;
    qDebug()<<result;
    qDebug()<<result->getJumpsReference().count();
    if(jumpNumber == -1 || jumpNumber >= result->getJumpsReference().count())
        result->getJumpsReference().push_back(jump);
    else{
        if(jumpNumber < 0)
            index = 0;

        jump.setSingleResult(result);
        result->getJumpsReference().insert(index, jump);
    }
    result->updatePointsSum();
}

void CompetitionResults::addJump(Team *team, JumpData &jump, int jumpNumber)
{
    CompetitionSingleResult *result = nullptr;
    for(auto & res : results){
        if(res.getTeam() == team){
            result = &res;
            break;
        }
    }
    if(result == nullptr){
        results.push_back(CompetitionSingleResult(competition, team, CompetitionSingleResult::TeamResult));
        jump.setSingleResult(&results.last());
        jump.setCompetition(results.last().getCompetition());
        result = &results.last();
    }
    int index = jumpNumber;
    if(jumpNumber == -1 || jumpNumber >= result->getJumpsReference().count())
        result->getJumpsReference().push_back(jump);
    else{
        if(jumpNumber < 0)
            index = 0;

        jump.setSingleResult(result);
        result->getJumpsReference().insert(index, jump);
    }
    result->updatePointsSum();
}

void CompetitionResults::addJump(CompetitionSingleResult *result, JumpData &jump, int jumpNumber)
{
    int index = jumpNumber;
    if(jumpNumber == -1 || jumpNumber >= result->getJumpsReference().count()){
        result->getJumpsReference().push_back(jump);
        jump.setCompetition(result->getJumpsReference().first().getCompetition());
    }
    else{
        if(jumpNumber < 0)
            index = 0;

        jump.setSingleResult(result);
        jump.setCompetition(result->getCompetition());
        result->getJumpsReference().insert(index, jump);
    }
    result->updatePointsSum();
}

int CompetitionResults::howManyJumpersJumpedInTeam(Team *team)
{
    CompetitionSingleResult * res = getResultOfTeam(team);
    int i = 0;
    QSet<Jumper *> jumpers;
    for(auto & jump : res->getJumpsReference())
    {
        if(jumpers.contains(jump.getJumper()) == false)
        {
            jumpers.insert(jump.getJumper());
            i++;
        }
    }
    return i;
}

int CompetitionResults::getIndexOfJumperResult(Jumper *jumper)
{
    int index = 0;
    for(auto & result : results)
    {
        if(result.getJumper() == jumper)
            return index;
        index++;
    }
    return -1;
}

void CompetitionResults::updatePositions()
{
    double previousResultPoints = 0;
    int actualPosition = 1;
    int add = 1;
    int i=0;
    for(auto & result : results){
        if(i==0){
            result.setPosition(1);
            previousResultPoints = result.getPointsSum();
            i++;
            continue;
        }

        if(previousResultPoints == result.getPointsSum())
        {
            result.setPosition(actualPosition);
            add += 1;
        }
        else{
            actualPosition += add;
            add = 1;
            result.setPosition(actualPosition);
        }

        previousResultPoints = result.getPointsSum();
        i++;
    }
    // for(auto & result : results){
    //   qDebug()<<result.getJumper()->getNameAndSurname()<<": "<<result.getPosition();
    //}
}

void CompetitionResults::sortInDescendingOrder()
{
    std::sort(results.begin(), results.end(), std::greater<CompetitionSingleResult>());
    updatePositions();
}

void CompetitionResults::sortInAscendingOrder()
{
    std::sort(results.begin(), results.end(), std::less<CompetitionSingleResult>());
    updatePositions();
}
