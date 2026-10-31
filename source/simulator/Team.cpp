#include "Team.h"
#include "../global/CountryCodes.h"
#include <QDebug>
#include <QJsonArray>
#include <QRandomGenerator>
#include <algorithm>
#include <cmath>
#include "../simulation-v2/QualityModel.h"

Team::Team(const QString &countryCode) : countryCode(CountryCodes::toFis(countryCode)),
    Identifiable()
{
}

void Team::arrangeSquad(int count,double variation,double k)
{
    if(count<1 || jumpers.isEmpty())return;
    const SimulationV2::QualityModel quality;
    QHash<Jumper *,double> rating;
    for(auto *jumper:jumpers){
        if(!jumper || !jumper->getSimulationProfile())return;
        const auto &profile=*jumper->getSimulationProfile();
        SimulationV2::Athlete athlete;athlete.overall=profile.overall;athlete.form=profile.form;
        rating.insert(jumper,quality.baseline(athlete,std::max(1.,k),0.,SimulationV2::EventKind::Competition).overallAndForm
            +(QRandomGenerator::global()->generateDouble()*2-1)*std::clamp(variation,0.,10.));
    }
    auto ranked=jumpers;
    std::stable_sort(ranked.begin(),ranked.end(),[&](auto *a,auto *b){return rating[a]>rating[b];});
    const int size=std::min(count,ranked.size());QVector<Jumper *> ordered;
    if(size>2)ordered.append(ranked[size/2]);
    for(int rank=size-1;rank>=0;--rank)if(size<=2 || rank!=size/2)ordered.append(ranked[rank]);
    for(int i=size;i<ranked.size();++i)ordered.append(ranked[i]);
    jumpers=ordered;
}

QVector<Team> Team::constructTeamsVectorByJumpersList(const QVector<Jumper> &jumpers, int minCount)
{
    QVector<Jumper *> pointers;pointers.reserve(jumpers.size());
    for(const auto &jumper:jumpers)pointers.append(const_cast<Jumper *>(&jumper));
    return constructTeamsVectorByJumpersList(pointers,minCount);
}

QVector<Team> Team::constructTeamsVectorByJumpersList(const QVector<Jumper *> &jumpers, int minCount)
{
    QVector<Team> teams;QHash<QString,int> indexes;
    for(auto *jumper:jumpers){
        if(!jumper)continue;
        const auto country=CountryCodes::toFis(jumper->getCountryCode());
        auto found=indexes.constFind(country);
        int index;
        if(found==indexes.cend()){index=teams.size();indexes.insert(country,index);teams.append(Team(country));}
        else index=found.value();
        teams[index].getJumpersReference().append(jumper);
    }
    teams.erase(std::remove_if(teams.begin(),teams.end(),[&](const Team &team){return team.getJumpers().size()<minCount;}),teams.end());
    for(auto &team:teams){auto &members=team.getJumpersReference();std::reverse(members.begin(),members.end());std::reverse(members.begin(),members.begin()+std::clamp(minCount,0,members.size()));}
    return teams;
}

QVector<Jumper *> Team::getJumpersFilteredByCountryCode(QVector<Jumper> &jumpers, const QString &countryCode)
{
    QVector<Jumper *> vector;
    for(auto & jumper : jumpers)
    {
        if(jumper.getCountryCode() == CountryCodes::toFis(countryCode))
            vector.push_back(&jumper);
    }
    return vector;
}

QVector<Jumper *> Team::getJumpersFilteredByCountryCode(QVector<Jumper *> &jumpers, const QString &countryCode)
{
    QVector<Jumper *> vector;
    for(auto & jumper : jumpers)
    {
        if(jumper->getCountryCode() == CountryCodes::toFis(countryCode))
            vector.push_back(jumper);
    }
    return vector;
}

void Team::cutTeamJumpers(Team *team, int jumpersCount)
{
    qDebug()<<team->getJumpersCount()<<", "<<jumpersCount;
    if(team->getJumpersCount() > jumpersCount){
        qDebug()<<team->getJumpersReference().count();
        team->getJumpersReference().erase(team->getJumpersReference().begin() + (jumpersCount), team->getJumpersReference().end());
        qDebug()<<team->getJumpersReference().count();
    }
}

Team *Team::getTeamByCountryCode(QVector<Team> *teams, QString countryCode)
{
    countryCode = CountryCodes::toFis(countryCode);
    for(auto & team : *teams)
    {
        if(team.getCountryCode() == countryCode)
            return &team;
    }
    return nullptr;
}

Team Team::getTeamObjectByCountryCode(QVector<Team> *teams, QString countryCode)
{
    countryCode = CountryCodes::toFis(countryCode);
    for(auto & team : *teams)
    {
        if(team.getCountryCode() == countryCode)
            return team;
    }
    return Team();
}

bool Team::containsTeamByCode(QVector<Team> &teams, QString code)
{
    code = CountryCodes::toFis(code);
    for(auto & t : teams)
    {
        if(t.getCountryCode() == code)
            return true;
    }
    return false;
}

bool Team::containsTeamByCode(QVector<Team *> &teams, QString code)
{
    code = CountryCodes::toFis(code);
    for(auto & t : teams)
    {
        if(t->getCountryCode() == code)
            return true;
    }
    return false;
}

QJsonObject Team::getJsonObject(Team &team)
{
    QJsonObject object;
    object.insert("id", team.getIDStr());
    object.insert("country-code", team.getCountryCode());
    QJsonArray jumpersArray;
    for(auto & jumper : team.getJumpersReference())
    {
        jumpersArray.push_back(jumper->getIDStr());
    }
    object.insert("jumpers-ids", jumpersArray);

    return object;
}

Team Team::getFromJson(QJsonObject json, IdentifiableObjectsStorage * storage)
{
    Team team;
    team.setID(sole::rebuild(json.value("id").toString().toStdString()));
    team.setCountryCode(json.value("country-code").toString());
    QJsonArray jumpersArray = json.value("jumpers-ids").toArray();
    for(auto jumperID : jumpersArray)
        team.getJumpersReference().push_back(static_cast<Jumper*>(storage->get(jumperID.toString())));

    return team;
}

QString Team::getCountryCode() const
{
    return countryCode;
}

void Team::setCountryCode(const QString &newCountryCode)
{
    countryCode = CountryCodes::toFis(newCountryCode);
}

void Team::setJumper(Jumper *jumper, int index)
{
    if(index < jumpers.count())
        jumpers[index] = jumper;
    else qDebug()<<"too high index! (Team::setJumper)";
}

QVector<Jumper *> Team::getJumpers() const
{
    return jumpers;
}

QVector<Jumper *> &Team::getJumpersReference()
{
    return jumpers;
}

void Team::setJumpers(const QVector<Jumper *> &newJumpers)
{
    jumpers = newJumpers;
}

int Team::getJumpersCount() const
{
    return jumpers.count();
}
