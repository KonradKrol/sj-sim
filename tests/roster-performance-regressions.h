#pragma once
// Frozen pre-optimization implementations provide reproducible timing baselines.
// Keep these confined to the benchmark; production code has one implementation.
static QVector<Team> previousTeamGrouping(const QVector<Jumper *> &jumpers, int minCount)
{
    qDebug()<<"minCount: "<<minCount;
    QVector<Team> teams;
    QStringList codes;
    for(const auto & jumper : jumpers)
    {
        if(codes.contains(jumper->getCountryCode().toUpper()) == true){
            for(auto & team : teams){
                if(team.getCountryCode() == jumper->getCountryCode())
                    team.getJumpersReference().push_back(jumper);
            }
        }
        else{
            codes.append(jumper->getCountryCode().toUpper());
            Team team;
            team.getJumpersReference().push_back(jumper);
            team.setCountryCode(jumper->getCountryCode());
            teams.append(team);
        }
    }
    while(true){
        bool next = false;
        int i=0;
        for(auto & team : teams){
            if(team.getJumpersReference().count() < minCount){
                teams.remove(i);
                next = true;
                break;
            }
            i++;
        }
        if(next == false)
            break;
    }

    for(auto & team : teams){
        std::reverse(team.getJumpersReference().begin(), team.getJumpersReference().end());
        int count = minCount;
        if(minCount > team.getJumpersReference().count())
            count = team.getJumpersReference().count();
        std::reverse(team.getJumpersReference().begin(), team.getJumpersReference().begin() + count);
    }
    return teams;
}
static QVector<Jumper *> previousQualification(CompetitionInfo *competition, QVector<Jumper *> jumpers)
{
    QVector<Jumper *> qualified;
    if(!competition || !competition->getAdvancementCompetition() || competition->getRulesPointer()->getRoundsReference().isEmpty())return qualified;
    auto *source = competition->getAdvancementCompetition();
    const auto &firstRound = competition->getRulesPointer()->getRoundsReference().first();
    const int limit = qMax(0,firstRound.getCount());
    if(limit == 0)return qualified;
    CompetitionResults standings = source->getResultsReference();
    standings.sortInDescendingOrder();
    const auto eligible = [](const CompetitionSingleResult &result) {
        const auto jumps = result.getJumps();
        return jumps.isEmpty() || (!jumps.last().getDNS() && !jumps.last().getDSQ());
    };
    auto &eligibleResults=standings.getResultsReference();
    for(int i=eligibleResults.size()-1;i>=0;--i)if(!eligible(eligibleResults[i]))eligibleResults.removeAt(i);
    standings.sortInDescendingOrder();
    if(firstRound.getKO()) {
        for(const auto &result : qAsConst(standings.getResultsReference())) {
            auto *jumper = result.getJumper();
            if(jumper && jumpers.contains(jumper) && eligible(result) && !qualified.contains(jumper))qualified.append(jumper);
            if(qualified.size() >= limit)break;
        }
        std::reverse(qualified.begin(),qualified.end());
        return qualified;
    }
    for(auto *jumper : qAsConst(jumpers)) {
        auto *result = standings.getResultOfIndividualJumper(jumper);
        if(!result || !eligible(*result) || qualified.contains(jumper))continue;
        bool protectedFall = false;
        const auto &attempts = result->getJumpsReference();
        if(source->getRulesPointer()->getHas95HSRule() && source->getHill() && !attempts.isEmpty()) {
            const auto &jump = attempts.last();
            protectedFall = jump.getLanding().getType() == Landing::Fall &&
                JumpSimulator::isJumperBeat95HSPercents(jump.getDistance(),source->getHill()->getHSPoint());
        }
        if((result->getPosition() > 0 && result->getPosition() <= limit) || protectedFall)qualified.append(jumper);
    }
    return qualified;
}
static void rosterPerformanceRegressions() {
    QVector<Jumper> actors;actors.reserve(12000);
    for(int i=0;i<12000;++i){const int code=i%160;QString country="X";country+=QChar('A'+code/26);country+=QChar('A'+code%26);actors.append(Jumper("Benchmark",QString::number(i),country));}
    QVector<Jumper *> pool;for(auto &actor:actors)pool.append(&actor);
    CompetitionInfo source,target;CompetitionRules rules;rules.setRounds({RoundInfo(1000,true)});source.setRules(rules);target.setRules(rules);target.setAdvancementCompetition(&source);
    for(int i=0;i<pool.size();++i){CompetitionSingleResult result(&source,pool[i]);result.setPointsSum(pool.size()-i);source.getResultsReference().getResultsReference().append(result);}
    auto measure=[](auto operation){QVector<qint64> times;for(int i=0;i<7;++i){QElapsedTimer timer;timer.start();operation();times.append(timer.nsecsElapsed());}std::sort(times.begin(),times.end());return times[3]/1000000.;};
    const auto oldTeams=measure([&]{check(previousTeamGrouping(pool,4).size()==160,"baseline country grouping");});
    const auto newTeams=measure([&]{check(Team::constructTeamsVectorByJumpersList(pool,4).size()==160,"optimized country grouping");});
    check(previousQualification(&target,pool)==IndividualCompetitionManager::getFilteredJumpersAfterQualifications(&target,pool),"qualification benchmark preserves exactly the baseline roster order");
    const auto oldQual=measure([&]{check(previousQualification(&target,pool).size()==1000,"baseline qualification");});
    const auto newQual=measure([&]{check(IndividualCompetitionManager::getFilteredJumpersAfterQualifications(&target,pool).size()==1000,"optimized qualification");});
    qInfo()<<"BENCHMARK: grouping ms"<<oldTeams<<"->"<<newTeams<<"qualification ms"<<oldQual<<"->"<<newQual;
}
