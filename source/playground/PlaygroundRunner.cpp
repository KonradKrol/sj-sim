#include <QObject>
#include "PlaygroundExperiment.h"
#include "../global/MyRandom.h"
#include "../global/GlobalSimulationSettings.h"
#include "../simulator/JumpSimulator.h"
#include "PlaygroundModel.h"
#include "../simulation-v2/ProfileBridge.h"
#include "../competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include "../competitions/CompetitionManagers/TeamCompetitionManager.h"
#include <QtConcurrent>
#include <QThreadPool>
#include <QElapsedTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QMap>
#include <QSysInfo>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <cmath>
#include <limits>
#include <vector>
#include <stdexcept>

namespace Playground {
namespace {
quint64 mix(quint64 x){x+=0x9e3779b97f4a7c15ULL;x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;x=(x^(x>>27))*0x94d049bb133111ebULL;return x^(x>>31);}
sole::uuid resultIdentity(quint64 seed,int competitor){const quint64 first=mix(seed^mix(quint64(competitor)+1)),second=mix(first^0x544945425245414bULL);const QString hex=QString::number(first,16).rightJustified(16,'0')+QString::number(second,16).rightJustified(16,'0');return sole::rebuild((hex.mid(0,8)+"-"+hex.mid(8,4)+"-"+hex.mid(12,4)+"-"+hex.mid(16,4)+"-"+hex.mid(20)).toStdString());}
struct Moment {
    qint64 n=0;long double sum=0,squares=0;double low=std::numeric_limits<double>::infinity(),high=-std::numeric_limits<double>::infinity();
    void add(double x){if(!std::isfinite(x))throw std::runtime_error("Non-finite simulation observation");++n;sum+=x;squares+=static_cast<long double>(x)*x;low=qMin(low,x);high=qMax(high,x);}
    void merge(const Moment &b){n+=b.n;sum+=b.sum;squares+=b.squares;low=qMin(low,b.low);high=qMax(high,b.high);}
    double mean()const{return n?double(sum/n):0;}
    double variance()const{return n>1?qMax(0.,double((squares-sum*sum/n)/(n-1))):0;}
    QJsonObject json()const{const double sd=std::sqrt(variance()),error=n>1?1.96*sd/std::sqrt(double(n)):0;return {{"count",double(n)},{"mean",n?QJsonValue(mean()):QJsonValue()},{"sd",n>1?QJsonValue(sd):QJsonValue()},{"min",n?QJsonValue(low):QJsonValue()},{"max",n?QJsonValue(high):QJsonValue()},{"ciLow",n>1?QJsonValue(mean()-error):QJsonValue()},{"ciHigh",n>1?QJsonValue(mean()+error):QJsonValue()}};}
};
struct Association {
    Moment x,y;long double xy=0;
    void add(double a,double b){x.add(a);y.add(b);xy+=static_cast<long double>(a)*b;}
    void merge(const Association &b){x.merge(b.x);y.merge(b.y);xy+=b.xy;}
    QJsonValue json()const{const long double vx=x.squares-x.sum*x.sum/qMax<qint64>(1,x.n),vy=y.squares-y.sum*y.sum/qMax<qint64>(1,y.n);return x.n>2 && vx>0 && vy>0?QJsonValue(qBound(-1.,double((xy-x.sum*y.sum/x.n)/std::sqrt(vx*vy)),1.)):QJsonValue();}
};
struct Distribution {
    Moment values,clusterTotals,clusterCounts;long double clusterProducts=0;QMap<int,qint64> bins;double width=2;
    void cluster(double total,int count){clusterTotals.add(total);clusterCounts.add(count);clusterProducts+=static_cast<long double>(total)*count;}
    void add(double x){values.add(x);++bins[int(std::floor(x/width))];}
    void merge(const Distribution &b){values.merge(b.values);clusterTotals.merge(b.clusterTotals);clusterCounts.merge(b.clusterCounts);clusterProducts+=b.clusterProducts;for(auto it=b.bins.begin();it!=b.bins.end();++it)bins[it.key()]+=it.value();}
    QJsonObject json()const{auto result=values.json();QJsonArray histogram;for(auto it=bins.begin();it!=bins.end();++it)histogram.append(QJsonObject{{"from",it.key()*width},{"to",(it.key()+1)*width},{"count",double(it.value())}});result["histogram"]=histogram;for(const auto &quantile:QVector<QPair<QString,double>>{{"p05",.05},{"p50",.5},{"p95",.95}}){qint64 accumulated=0;QJsonValue estimate;for(auto it=bins.begin();it!=bins.end();++it){accumulated+=it.value();if(accumulated>=qMax<qint64>(1,qint64(std::ceil(values.n*quantile.second)))){estimate=(it.key()+.5)*width;break;}}result[quantile.first]=estimate;}result["quantileBinWidth"]=width;if(clusterTotals.n){result["ciUnit"]="competition";result["clusters"]=double(clusterTotals.n);if(clusterTotals.n>1 && clusterCounts.sum>0){const long double mean=values.mean();const long double k=clusterTotals.n;const long double xx=clusterTotals.squares-clusterTotals.sum*clusterTotals.sum/k,yy=clusterCounts.squares-clusterCounts.sum*clusterCounts.sum/k,xy=clusterProducts-clusterTotals.sum*clusterCounts.sum/k;const long double residual=qMax(0.L,xx-2*mean*xy+mean*mean*yy);const double se=double(std::sqrt(residual*clusterTotals.n/(clusterTotals.n-1))/clusterCounts.sum);result["ciLow"]=values.mean()-1.96*se;result["ciHigh"]=values.mean()+1.96*se;}else {result["ciLow"]=QJsonValue();result["ciHigh"]=QJsonValue();}}return result;}
};
struct AthleteStats {
    Moment place,total,wind,pairedPlace,pairedPoints,pairedDistance;Distribution distance,points;QVector<qint64> places;qint64 starts=0,dsq=0,falls=0,jumps=0,advanced=0,wins=0,podiums=0,teamDsq=0;
    void merge(const AthleteStats &b){place.merge(b.place);total.merge(b.total);distance.merge(b.distance);points.merge(b.points);wind.merge(b.wind);pairedPlace.merge(b.pairedPlace);pairedPoints.merge(b.pairedPoints);pairedDistance.merge(b.pairedDistance);for(int i=0;i<places.size();++i)places[i]+=b.places[i];starts+=b.starts;dsq+=b.dsq;falls+=b.falls;jumps+=b.jumps;advanced+=b.advanced;wins+=b.wins;podiums+=b.podiums;teamDsq+=b.teamDsq;}
    QJsonObject json(int index,const QString &name)const{QJsonArray histogram;for(int i=1;i<places.size();++i)if(places[i])histogram.append(QJsonObject{{"place",i},{"count",double(places[i])}});return {{"index",index},{"name",name},{"starts",double(starts)},{"jumps",double(jumps)},{"dsq",double(dsq)},{"teamDsq",double(teamDsq)},{"falls",double(falls)},{"advanced",double(advanced)},{"place",place.json()},{"total",total.json()},{"distance",distance.json()},{"points",points.json()},{"wind",wind.json()},{"places",histogram},{"pairedPlace",pairedPlace.json()},{"pairedPoints",pairedPoints.json()},{"pairedDistance",pairedDistance.json()},{"wins",double(wins)},{"podiums",double(podiums)}};}
};
struct JumpRow {
    int athlete=0,round=0,group=0;double distance=0,points=0,wind=0,compensation=0,judges=0,quality=0,random=0,experienced=0,startSeconds=0;bool dsq=false,fall=false;QJsonObject detail;
    QJsonObject json()const{return {{"athlete",athlete},{"round",round},{"group",group},{"distance",distance},{"points",points},{"wind",wind},{"compensation",compensation},{"judges",judges},{"quality",quality},{"random",random},{"experiencedWind",experienced},{"measurementError",wind-experienced},{"startSeconds",startSeconds},{"dsq",dsq},{"fall",fall},{"detail",detail}};}
};
struct Trial {QVector<JumpRow> jumps;QVector<int> places;QVector<double> totals;QVector<bool> eligible,teamDisqualified;int gate=0,fieldSize=0;bool team=false;};
QVector<Jumper> makeAthletes(const QJsonObject &config,const QJsonObject &p,int variantIndex=0) {
    QVector<Jumper> result;const auto roster=config["roster"].toArray();const int count=config["athletes"].toInt(),focus=config["focus"].toInt();
    const auto changes=config["variants"].toArray()[variantIndex].toObject()["changes"].toObject();
    const auto explicitValue=[&](const QString &key){return config["parameters"].toObject().contains(key) || changes.contains(key);};
    for(int i=0;i<count;++i){
        AthleteProfile profile;
        if(!roster.isEmpty())profile=*Jumper::getFromJson(roster[i].toObject()).getSimulationProfile();
        else {profile=AthleteProfile::create();profile.name=QStringLiteral("Jumper");profile.surname=QString::number(i+1);profile.overall=qBound(1,int(std::round(p["field.skill"].toDouble()+p["field.spread"].toDouble()*(.5-double(i)/qMax(1,count-1)))),100);}
        if(roster.isEmpty() || explicitValue("field.form"))profile.form=p["field.form"].toInt();
        if(roster.isEmpty() || explicitValue("field.landing"))profile.landing=p["field.landing"].toInt();
        for(int j=0;j<3;++j){auto key=QString("field.perk.%1").arg(j);if(roster.isEmpty() || explicitValue(key))profile.perks[j]=p[key].toInt();}
        if(i==focus){auto input=athleteInput(p,"athlete");profile.overall=input.overall;profile.form=input.form;profile.landing=input.landing;profile.perks=input.perks;}
        Jumper athlete;QString error;if(!athlete.setSimulationProfile(profile,error))throw std::invalid_argument(error.toStdString());result.append(athlete);
    }
    return result;
}
Hill makeHill(const QJsonObject &config,const QJsonObject &p){
    HillProfile profile;auto source=config["hill"].toObject()["source"];
    if(source.isObject())profile=*Hill::getFromJson(source.toObject()).getSimulationProfile();else{profile=HillProfile::create();profile.name=config["hill"].toObject()["name"].toString();}
    profile.k=p["hill.k"].toDouble();profile.hs=p["hill.hs"].toDouble();profile.pointsAtK=p["hill.pointsAtK"].toDouble();profile.pointsPerMetre=p["hill.pointsPerMetre"].toDouble();profile.gatePoints=p["hill.gatePoints"].toDouble();profile.headwindPoints=p["hill.headwindPoints"].toDouble();profile.tailwindPoints=p["hill.tailwindPoints"].toDouble();
    Hill hill;QString error;if(!hill.setSimulationProfile(profile,error))throw std::invalid_argument(error.toStdString());return hill;
}
CompetitionRules makeRules(const QJsonObject &config,const QJsonObject &p){CompetitionRules rules;if(config["rulesData"].isObject())rules=CompetitionRules::getFromJson(config["rulesData"].toObject());else{QVector<RoundInfo> rounds;for(const auto &entry:config["rounds"].toArray())rounds.append(RoundInfo(entry.toInt(),true));rules.setRounds(rounds);rules.setHasWindCompensations(true);rules.setHasGateCompensations(true);rules.setHasJudgesPoints(true);rules.setHasDsq(true);rules.setHas95HSRule(true);}const auto set=[&](const char *key,bool fallback){return p.contains(key)?p[key].toInt()!=0:fallback;};rules.setHasDsq(set("rules.dsq",rules.getHasDsq()));rules.setWindMeasurementType(p["rules.measurement"].toInt());rules.setHasWindCompensations(set("rules.windCompensation",rules.getHasWindCompensations()));rules.setHasGateCompensations(set("rules.gateCompensation",rules.getHasGateCompensations()));rules.setHasJudgesPoints(set("rules.judges",rules.getHasJudgesPoints()));rules.setHas95HSRule(set("rules.95hs",rules.getHas95HSRule()));for(auto &round:rules.getRoundsReference())if(round.getKO() && round.getKoGroupsSelectionType()==CompetitionRules::Manual)round.setKoGroupsSelectionType(CompetitionRules::Random);rules.setHillRecordBreaking(false);return rules;}
int autoGate(const QJsonObject &config){
    auto p=valuesFor(config,0);if(p["gate"].toInt()>0)return p["gate"].toInt();
    auto athletes=makeAthletes(config,p);SimulationV2::JumpRequest r;r.hill=hillInput(p);r.event=SimulationV2::EventKind(p["event"].toInt());
    double best=-1e99;SimulationV2::Engine engine(engineParameters(p));
    for(const auto &athlete:athletes){auto a=SimulationV2::athleteInput(*athlete.getSimulationProfile());double q=engine.quality().baseline(a,r.hill.k,r.hill.takeoffBias,r.event).value();if(q>best){best=q;r.athlete=a;}}
    SimulationV2::ContinuousWeather weather(mix(config["seed"].toString().toULongLong()),windParameters(p),precipitationParameters(p));
    int lo=0,hi=200;while(lo<hi){int mid=(lo+hi+1)/2;r.scoring.juryGate=r.scoring.referenceGate=mid;if(engine.previewDistance(r,weather)<=r.hill.hs)lo=mid;else hi=mid-1;}return lo;
}
Trial simulate(const QJsonObject &config,int variant,int index,int commonGate,const std::shared_ptr<Control> &control) {
    const auto p=valuesFor(config,variant);const quint64 root=config["seed"].toString().toULongLong(),contestSeed=mix(root^mix(quint64(index)));MyRandom::ScopedSeed contestRandom(contestSeed);
    auto athletes=makeAthletes(config,p,variant);auto hill=makeHill(config,p);CompetitionInfo info(&hill);info.setRules(makeRules(config,p));auto *rules=info.getRulesPointer();info.getRoundsKOGroupsReference().reserve(rules->getRounds().size()+1);auto &results=info.getResultsReference();QHash<Jumper *,int> indices;QVector<Jumper *> pointers;for(int i=0;i<athletes.size();++i){indices.insert(&athletes[i],i);pointers.append(&athletes[i]);}
    Trial trial;trial.team=rules->getCompetitionType()==CompetitionRules::Team;trial.gate=p["gate"].toInt()>0?p["gate"].toInt():commonGate;trial.places.fill(0,athletes.size());trial.totals.fill(0,athletes.size());trial.eligible.fill(false,athletes.size());trial.teamDisqualified.fill(false,athletes.size());trial.fieldSize=athletes.size();
    IndividualCompetitionManager individual;TeamCompetitionManager teamManager;AbstractCompetitionManager *manager=trial.team?static_cast<AbstractCompetitionManager *>(&teamManager):&individual;
    manager->setCompetitionInfo(&info);manager->setCompetitionRules(rules);manager->setResults(&results);manager->setActualGate(trial.gate);manager->setActualRound(1);manager->setRoundsStartingGates({trial.gate});
    QVector<Team> teams;QVector<Team *> teamPointers;
    if(trial.team){teams=Team::constructTeamsVectorByJumpersList(pointers,rules->getJumpersInTeamCount());for(auto &entry:teams){Team::cutTeamJumpers(&entry,rules->getJumpersInTeamCount());teamPointers.append(&entry);}if(teams.size()<2)throw std::runtime_error("Team experiment needs at least two complete national teams");trial.fieldSize=teams.size();for(const auto &team:teams)for(auto *athlete:team.getJumpers())trial.eligible[indices.value(athlete)]=true;teamManager.setRoundsTeams({teamPointers});teamManager.setActualGroup(1);teamManager.setupStartListStatusesForActualRound(true);}
    else {QVector<KOGroup> groups;if(rules->getRounds().first().getKO())groups=KOGroup::constructKOGroups(&rules->getRoundsReference()[0],&pointers,rules->getRounds().first().getKoGroupsSelectionType(),&info);info.getRoundsKOGroupsReference().append(groups);individual.getRoundsKOGroupsReference().append(&info.getRoundsKOGroupsReference().last());if(rules->getRounds().first().getKO())pointers=KOGroup::getJumpersFromGroups(&info.getRoundsKOGroupsReference().last());if(pointers.isEmpty())throw std::runtime_error("Rules produced no KO starters; check the group size and selection method");for(auto *athlete:pointers)trial.eligible[indices.value(athlete)]=true;individual.setFirstRoundJumpers(pointers);individual.setupStartListStatusesForActualRound();}
    SimulationV2::Engine engine(engineParameters(p));
    SimulationV2::ContinuousWeather weather(mix(contestSeed^0x57494e44ULL),windParameters(p),precipitationParameters(p));
    SimulationV2::FieldWindMeasurement measurement(p["hill.k"].toDouble(),*rules,measurementParameters(p));
    SimulationV2::JumpRequest request;request.hill=hillInput(p);request.event=SimulationV2::EventKind(p["event"].toInt());request.dsqProbability=rules->getHasDsq()?p["dsq"].toDouble():0;
    request.scoring.juryGate=trial.gate;request.scoring.referenceGate=commonGate;
    request.scoring.judges=rules->getHasJudgesPoints();request.scoring.windCompensation=rules->getHasWindCompensations();request.scoring.gateCompensation=rules->getHasGateCompensations();
    double time=0;
    std::unique_ptr<KORoundManager> knockout;
    for(int round=1;round<=rules->getRounds().size();++round){
        if(control && control->canceled.load(std::memory_order_relaxed))return {};
        if(!trial.team && rules->getRounds()[round-1].getKO()) {if(individual.getRoundsKOGroupsReference().size()<round)throw std::runtime_error("Missing KO groups");knockout=std::make_unique<KORoundManager>(individual.getRoundsKOGroupsReference()[round-1],&results,&rules->getRoundsReference()[round-1],&individual);individual.setKOManager(knockout.get());knockout->setLuckyLosersCount(round<rules->getRounds().size()?qMax(0,rules->getRounds()[round].getCount()-int(individual.getRoundsKOGroupsReference()[round-1]->size())*rules->getRounds()[round-1].getAdvancingFromKOGroup()):0);}else {individual.setKOManager(nullptr);knockout.reset();}
        const int groups=trial.team?rules->getJumpersInTeamCount():1;
        for(int group=1;group<=groups;++group){
            for(auto &status:manager->getStartListStatusesReference()){
                if(control && control->canceled.load(std::memory_order_relaxed))return {};
                if(status.getJumpStatus()!=StartListCompetitorStatus::Unfinished)continue;
                auto *athlete=status.getJumper();const int athleteIndex=indices.value(athlete,-1);if(athleteIndex<0)throw std::runtime_error("Unknown athlete in experiment start list");Team *currentTeam=nullptr;if(trial.team){for(auto *entry:teamManager.getActualRoundTeamsReference())if(entry->getJumpers().contains(athlete)){currentTeam=entry;break;}if(!currentTeam)throw std::runtime_error("Missing experiment team");if(auto *standing=results.getResultOfTeam(currentTeam);standing && standing->isTeamDisqualified()){status.setJumpStatus(StartListCompetitorStatus::Dsq);continue;}}
                const quint64 jumpSeed=mix(contestSeed^mix(quint64(round)*1024+quint64(athleteIndex)));
                request.athlete=SimulationV2::athleteInput(*athlete->getSimulationProfile());request.startSeconds=p["clock.mode"].toInt()==0?0:time;
                request.captureExposure=athleteIndex==config["focus"].toInt() && (!control || index<config["sampleLimit"].toInt());
                SimulationV2::SeededRandom rng(mix(jumpSeed^0x4a554d50ULL));auto r=engine.simulate(request,rng,weather,measurement);time+=p["clock.interval"].toDouble();
                JumpData jump(athlete,&hill);jump.setCompetition(&info);jump.setDistance(r.measuredDistance);jump.setPoints(r.score.total);jump.setDSQ(r.status==SimulationV2::JumpStatus::Disqualified);jump.setLanding(Landing(int(r.landing)));jump.setAveragedWind(r.score.measuredWind);jump.setJudgesPoints(r.score.judges);jump.setWindCompensation(r.score.wind);
                results.addJump(athlete,jump);results.getResultOfIndividualJumper(athlete)->setID(resultIdentity(contestSeed,athleteIndex));
                status.setJumpStatus(jump.getDSQ()?StartListCompetitorStatus::Dsq:StartListCompetitorStatus::Finished);status.setQualifiedBy95HSRule(r.landing==SimulationV2::LandingKind::Fall && r.measuredDistance>=.95*request.hill.hs && rules->getHas95HSRule());
                JumpRow row;row.athlete=athleteIndex;row.round=round;row.group=group;row.distance=r.measuredDistance;row.points=r.score.total;row.wind=r.score.measuredWind;row.compensation=r.score.wind;row.judges=r.score.judges;row.quality=r.quality.value();row.random=r.quality.randomEffect;row.experienced=r.flight.experiencedWind.head;row.startSeconds=request.startSeconds;row.dsq=jump.getDSQ();row.fall=r.landing==SimulationV2::LandingKind::Fall;
                if(request.captureExposure){QJsonArray sections,marks;for(const auto &e:r.flight.exposures)sections.append(QJsonObject{{"begin",e.begin},{"end",e.end},{"traversed",e.traversed},{"head",e.experienced.head},{"cross",e.experienced.cross},{"minimumHead",e.minimumHead},{"maximumHead",e.maximumHead}});for(double v:r.judges)marks.append(v);row.detail={{"windResponse",QJsonObject{{"variation",r.flight.windResponse.variation},{"adverseQuantile",r.flight.windResponse.adverseQuantile},{"severity",r.flight.windResponse.severity}}},{"engineVersion",2},{"seed",QString::number(mix(jumpSeed^0x4a554d50ULL))},{"exposures",sections},{"marks",marks},{"landing",int(r.landing)},{"duration",r.flight.duration},{"crosswind",r.flight.meanAbsoluteCrosswind},{"rawDistance",r.flight.distance}};}
                trial.jumps.append(row);if(control)control->jumps.fetch_add(1,std::memory_order_relaxed);
            }
            if(trial.team && group<groups)teamManager.setupNextGroup();
        }
        manager->updateCompetitorsAdvanceStatuses();
        if(round<rules->getRounds().size()){contestRandom.reseed(mix(contestSeed^0x47524f5550ULL^quint64(round)));if(trial.team){teamManager.setupNextRound();if(teamManager.getActualRoundTeamsReference().isEmpty())break;}else {individual.setupNextRound();if(individual.getActualRoundJumpersReference().isEmpty())break;}}
    }
    results.sortInDescendingOrder();
    for(const auto &standing:results.getResultsReference()){
        if(trial.team){for(auto *athlete:standing.getTeam()->getJumpers()){const int i=indices.value(athlete,-1);if(i>=0){trial.places[i]=standing.getPosition();trial.totals[i]=standing.getPointsSum();trial.teamDisqualified[i]=standing.isTeamDisqualified();}}}
        else {const int i=indices.value(standing.getJumper(),-1);if(i>=0){trial.places[i]=standing.getPosition();trial.totals[i]=standing.getPointsSum();}}
    }
    return trial;
}
struct Aggregate {
    qint64 competitions=0,jumps=0,dsq=0,falls=0;Distribution distance,points,focusDistance,focusPoints;Association windDistance,windPoints,focusWindPlace;QVector<AthleteStats> athletes;Moment pairedPlace,pairedPoints,pairedDistance;QJsonArray samples;QMap<int,Distribution> roundDistances;
    Aggregate(int count=0){points.width=5;focusPoints.width=5;athletes.resize(count);for(auto &athlete:athletes){athlete.places.fill(0,count+2);athlete.points.width=5;}}
    void add(const Trial &trial,int focus,int index,int sampleLimit,const Trial *baseline){
        ++competitions;QVector<int> counts(athletes.size(),0),rounds(athletes.size(),0);QVector<double> windSums(athletes.size(),0),distanceSums(athletes.size(),0),pointsSums(athletes.size(),0);QVector<int> validCounts(athletes.size(),0);QMap<int,double> roundSums;QMap<int,int> roundCounts;double allMeters=0,allPoints=0;int validJumps=0;
        for(const auto &jump:trial.jumps){auto &athlete=athletes[jump.athlete];++jumps;++athlete.jumps;++counts[jump.athlete];rounds[jump.athlete]=qMax(rounds[jump.athlete],jump.round);windSums[jump.athlete]+=jump.wind;if(jump.dsq){++dsq;++athlete.dsq;continue;}if(jump.fall){++falls;++athlete.falls;}distance.add(jump.distance);points.add(jump.points);allMeters+=jump.distance;allPoints+=jump.points;++validJumps;distanceSums[jump.athlete]+=jump.distance;pointsSums[jump.athlete]+=jump.points;++validCounts[jump.athlete];roundSums[jump.round]+=jump.distance;++roundCounts[jump.round];athlete.distance.add(jump.distance);athlete.points.add(jump.points);athlete.wind.add(jump.wind);windDistance.add(jump.wind,jump.distance);windPoints.add(jump.wind,jump.points);roundDistances[jump.round].add(jump.distance);if(jump.athlete==focus){focusDistance.add(jump.distance);focusPoints.add(jump.points);}}
        distance.cluster(allMeters,validJumps);points.cluster(allPoints,validJumps);focusDistance.cluster(distanceSums[focus],validCounts[focus]);focusPoints.cluster(pointsSums[focus],validCounts[focus]);for(auto it=roundSums.begin();it!=roundSums.end();++it)roundDistances[it.key()].cluster(it.value(),roundCounts[it.key()]);
        for(int i=0;i<athletes.size();++i)if(counts[i] || (trial.team && trial.eligible[i])){auto &athlete=athletes[i];athlete.distance.cluster(distanceSums[i],validCounts[i]);athlete.points.cluster(pointsSums[i],validCounts[i]);++athlete.starts;if(trial.teamDisqualified.value(i))++athlete.teamDsq;athlete.total.add(trial.totals[i]);if(rounds[i]>1)++athlete.advanced;const int place=trial.places[i]>0?trial.places[i]:trial.fieldSize+1;++athlete.places[place];if(trial.places[i]>0){athlete.place.add(place);if(place==1)++athlete.wins;if(place<=3)++athlete.podiums;}if(i==focus && trial.places[i]>0 && counts[i]>0)focusWindPlace.add(windSums[i]/counts[i],place);}
        if(baseline){QVector<double> baseSums(athletes.size(),0);QVector<int> baseCounts(athletes.size(),0);for(const auto &jump:baseline->jumps)if(!jump.dsq){baseSums[jump.athlete]+=jump.distance;++baseCounts[jump.athlete];}for(int i=0;i<athletes.size();++i){auto &athlete=athletes[i];if(trial.places[i]>0 && baseline->places[i]>0)athlete.pairedPlace.add(trial.places[i]-baseline->places[i]);if(trial.eligible.value(i) && baseline->eligible.value(i))athlete.pairedPoints.add(trial.totals[i]-baseline->totals[i]);if(validCounts[i] && baseCounts[i])athlete.pairedDistance.add(distanceSums[i]/validCounts[i]-baseSums[i]/baseCounts[i]);}pairedPlace=athletes[focus].pairedPlace;pairedPoints=athletes[focus].pairedPoints;pairedDistance=athletes[focus].pairedDistance;}
        if(index<sampleLimit){QJsonArray focusRows;for(const auto &jump:trial.jumps)if(jump.athlete==focus)focusRows.append(jump.json());samples.append(QJsonObject{{"competition",index},{"place",trial.places[focus]},{"points",trial.totals[focus]},{"gate",trial.gate},{"jumps",focusRows}});}
    }
    void merge(const Aggregate &b){competitions+=b.competitions;jumps+=b.jumps;dsq+=b.dsq;falls+=b.falls;distance.merge(b.distance);points.merge(b.points);focusDistance.merge(b.focusDistance);focusPoints.merge(b.focusPoints);windDistance.merge(b.windDistance);windPoints.merge(b.windPoints);focusWindPlace.merge(b.focusWindPlace);pairedPlace.merge(b.pairedPlace);pairedPoints.merge(b.pairedPoints);pairedDistance.merge(b.pairedDistance);for(int i=0;i<athletes.size();++i)athletes[i].merge(b.athletes[i]);for(const auto &sample:b.samples)samples.append(sample);for(auto it=b.roundDistances.begin();it!=b.roundDistances.end();++it)roundDistances[it.key()].merge(it.value());}
};
QJsonArray insights(const Aggregate &a,const Aggregate &base,int focus){QJsonArray notes;const auto &athlete=a.athletes[focus];if(a.competitions<100)notes.append(QJsonObject{{"kind","sample"},{"text",QObject::tr("Mała próba: zwiększ liczbę konkursów przed oceną drobnych różnic.")}});const auto paired=a.pairedPlace.json();if(a.pairedPlace.n>1){const bool clear=paired["ciLow"].toDouble()>0 || paired["ciHigh"].toDouble()<0;notes.append(QJsonObject{{"kind","pairedPlace"},{"text",clear?QObject::tr("Zmiana średniego miejsca wybranego skoczka ma przedział 95% po jednej stronie zera."):QObject::tr("Próba nie rozstrzyga kierunku zmiany średniego miejsca wybranego skoczka.")},{"effect",paired}});}if(a.jumps)notes.append(QJsonObject{{"kind","falls"},{"rate",double(a.falls)/a.jumps},{"baselineRate",base.jumps?double(base.falls)/base.jumps:0},{"text",QObject::tr("Częstość upadków: udział w oddanych skokach, razem z DSQ w mianowniku.")}});if(athlete.starts && athlete.place.n<athlete.starts)notes.append(QJsonObject{{"kind","unclassified"},{"count",double(athlete.starts-athlete.place.n)},{"text",QObject::tr("Nieklasyfikowane starty nie są wliczane do średniego miejsca. Są osobno w histogramie.")}});notes.append(QJsonObject{{"kind","association"},{"text",QObject::tr("Korelacje opisują zależność w tej próbie. Efekt zmiany parametru oceniaj przez kontrolowane warianty.")}});return notes;}
}
QString engineBuild(){
    static const QString build=[] {
#ifdef Q_OS_LINUX
        QFile executable(QStringLiteral("/proc/self/exe"));
#else
        QFile executable(QCoreApplication::applicationFilePath());
#endif
        QCryptographicHash hash(QCryptographicHash::Sha256);
        const QString prefix=QStringLiteral("playground-v2 / ")+QString::fromLatin1(qVersion())+" / "+QSysInfo::buildAbi();
        if(executable.open(QIODevice::ReadOnly) && hash.addData(&executable))return prefix+" / sha256:"+QString::fromLatin1(hash.result().toHex());
        return prefix+QStringLiteral(" / unverified / " __DATE__ " " __TIME__);
    }();return build;
}
QJsonObject replay(const QJsonObject &config,int variant,int competition){QString error;if(!validate(config,error))throw std::invalid_argument(error.toStdString());if(variant<0 || variant>=config["variants"].toArray().size() || competition<0 || competition>=config["competitions"].toInt())throw std::out_of_range("Replay index outside experiment");const auto trial=simulate(config,variant,competition,autoGate(config),{});auto p=valuesFor(config,variant);const auto athletes=makeAthletes(config,p,variant);QJsonArray rows,jumps;for(int i=0;i<athletes.size();++i)rows.append(QJsonObject{{"athlete",i},{"name",athletes[i].getNameAndSurname()},{"eligible",trial.eligible[i]},{"country",athletes[i].getCountryCode()},{"teamDsq",trial.teamDisqualified[i]},{"place",trial.places[i]},{"points",trial.totals[i]}});for(const auto &jump:trial.jumps)jumps.append(jump.json());return {{"engineBuild",engineBuild()},{"variant",variant},{"competition",competition},{"gate",trial.gate},{"team",trial.team},{"standings",rows},{"jumps",jumps}};}
QJsonObject run(const QJsonObject &config,const std::shared_ptr<Control> &provided){QString error;if(!validate(config,error))throw std::invalid_argument(error.toStdString());auto control=provided?provided:std::make_shared<Control>();const int count=config["competitions"].toInt(),athleteCount=config["athletes"].toInt(),focus=config["focus"].toInt(),sampleLimit=config["sampleLimit"].toInt(),cases=config["variants"].toArray().size();const qint64 perWorker=qMax<qint64>(1,qint64(cases)*athleteCount*(qint64(athleteCount+2)*sizeof(qint64)+32768));const int workers=qMax(1,qMin(qMin(config["threads"].toInt(),count),int(qMax<qint64>(1,512*1024*1024/perWorker))));QElapsedTimer timer;timer.start();const int gate=autoGate(config);std::vector<Aggregate> totals;for(int i=0;i<cases;++i)totals.emplace_back(athleteCount);QMutex mutex;std::atomic_int next{0};std::exception_ptr exception;QThreadPool pool;pool.setMaxThreadCount(workers);std::vector<QFuture<void>> futures;
    for(int worker=0;worker<workers;++worker)futures.push_back(QtConcurrent::run(&pool,[&]{try{std::vector<Aggregate> local;for(int c=0;c<cases;++c)local.emplace_back(athleteCount);while(!control->canceled.load(std::memory_order_relaxed)){const int index=next.fetch_add(1,std::memory_order_relaxed);if(index>=count)break;std::vector<Trial> trials;bool finished=true;for(int c=0;c<cases;++c){Trial trial;try{trial=simulate(config,c,index,gate,control);}catch(const std::exception &error){throw std::runtime_error(QString("Wariant %1, konkurs %2: %3").arg(config["variants"].toArray()[c].toObject()["name"].toString()).arg(index+1).arg(QString::fromUtf8(error.what())).toStdString());}if(control->canceled.load(std::memory_order_relaxed)){finished=false;break;}trials.push_back(std::move(trial));}if(!finished)break;for(int c=0;c<cases;++c)local[c].add(trials[c],focus,index,sampleLimit,c?&trials[0]:nullptr);control->completed.fetch_add(1,std::memory_order_relaxed);}QMutexLocker guard(&mutex);for(int c=0;c<cases;++c)totals[c].merge(local[c]);}catch(...){QMutexLocker guard(&mutex);if(!exception)exception=std::current_exception();control->canceled.store(true,std::memory_order_relaxed);}}));for(auto &future:futures)future.waitForFinished();if(exception)std::rethrow_exception(exception);
    const auto p=valuesFor(config,0);const auto athletes=makeAthletes(config,p);int fieldSize=athletes.size();if(makeRules(config,p).getCompetitionType()==CompetitionRules::Team){QHash<QString,int> countries;for(const auto &athlete:athletes)++countries[athlete.getCountryCode()];fieldSize=0;for(auto count:countries)if(count>=makeRules(config,p).getJumpersInTeamCount())++fieldSize;}QJsonArray variants;
    for(int c=0;c<cases;++c){auto &aggregate=totals[c];QJsonArray standings,rounds;for(int i=0;i<athletes.size();++i)standings.append(aggregate.athletes[i].json(i,athletes[i].getNameAndSurname()));for(auto it=aggregate.roundDistances.begin();it!=aggregate.roundDistances.end();++it)rounds.append(QJsonObject{{"round",it.key()},{"distance",it.value().json()}});std::vector<QJsonObject> sorted;for(const auto &sample:aggregate.samples)sorted.push_back(sample.toObject());std::sort(sorted.begin(),sorted.end(),[](const QJsonObject &a,const QJsonObject &b){return a["competition"].toInt()<b["competition"].toInt();});QJsonArray samples;for(const auto &sample:sorted)samples.append(sample);
        variants.append(QJsonObject{{"name",config["variants"].toArray()[c].toObject()["name"]},{"parameters",valuesFor(config,c)},{"competitions",double(aggregate.competitions)},{"jumps",double(aggregate.jumps)},{"dsq",double(aggregate.dsq)},{"falls",double(aggregate.falls)},{"distance",aggregate.distance.json()},{"points",aggregate.points.json()},{"focusDistance",aggregate.focusDistance.json()},{"focusPoints",aggregate.focusPoints.json()},{"windDistanceCorrelation",aggregate.windDistance.json()},{"windPointsCorrelation",aggregate.windPoints.json()},{"focusWindPlaceCorrelation",aggregate.focusWindPlace.json()},{"athletes",standings},{"focus",standings[focus]},{"rounds",rounds},{"pairedPlace",aggregate.pairedPlace.json()},{"pairedPoints",aggregate.pairedPoints.json()},{"pairedDistance",aggregate.pairedDistance.json()},{"samples",samples},{"insights",insights(aggregate,totals[0],focus)}});
    }
    QJsonArray targets;for(const auto &row:config["targets"].toArray()){auto t=row.toObject();auto metric=variants[t["variant"].toInt()].toObject()[t["metric"].toString()].toObject();t["result"]=metric;QString state="inconclusive";if(metric["count"].toInt()>1 && metric["ciLow"].isDouble()){double lo=metric["ciLow"].toDouble(),hi=metric["ciHigh"].toDouble();if(lo>=t["minimum"].toDouble() && hi<=t["maximum"].toDouble())state="pass";else if(hi<t["minimum"].toDouble() || lo>t["maximum"].toDouble())state="fail";}t["state"]=state;targets.append(t);}
    return {{"schema",2},{"engineBuild",engineBuild()},{"config",config},{"variants",variants},{"completed",control->completed.load()},{"canceled",control->canceled.load()},{"threadsUsed",workers},{"elapsedMs",double(timer.elapsed())},{"commonGate",gate},{"fieldSize",fieldSize},{"manualGroupsPolicy","seeded random for batch contests"},{"competitionType",makeRules(config,p).getCompetitionType()==CompetitionRules::Team?"team":"individual"},{"autoGateAtBound",gate==0 || gate==200},{"targets",targets},{"engineVersion",2},{"seedPolicy",QStringLiteral("Per competition, round and athlete; independent wind and jump streams. Same binary required for exact numeric reproduction.")},{"comparisonPolicy",QStringLiteral("Paired differences are variant minus baseline. Place excludes unclassified results; totals are compared after each variant's actual round progression.")}};
}
}
