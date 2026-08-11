#include "Team.h"
#include <QDebug>
#include <QJsonArray>

Team::Team(const QString &countryCode) : countryCode(countryCode),
    Identifiable()
{
}

QVector<Team> Team::constructTeamsVectorByJumpersList(const QVector<Jumper> &jumpers, int minCount)
{
    qDebug()<<"minCount: "<<minCount;
    QVector<Team> teams;
    QStringList codes;
    for(const auto & jumper : jumpers)
    {
        if(codes.contains(jumper.getCountryCode().toUpper()) == true){
            for(auto & team : teams){
                if(team.getCountryCode() == jumper.getCountryCode())
                    team.getJumpersReference().push_back(const_cast<Jumper *>(&jumper));
            }
        }
        else{
            codes.append(jumper.getCountryCode().toUpper());
            Team team;
            team.getJumpersReference().push_back(const_cast<Jumper *>(&jumper));
            team.setCountryCode(jumper.getCountryCode());
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

QVector<Team> Team::constructTeamsVectorByJumpersList(const QVector<Jumper *> &jumpers, int minCount)
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

QVector<Jumper *> Team::getJumpersFilteredByCountryCode(QVector<Jumper> &jumpers, const QString &countryCode)
{
    QVector<Jumper *> vector;
    for(auto & jumper : jumpers)
    {
        if(jumper.getCountryCode().toUpper() == countryCode.toUpper())
            vector.push_back(&jumper);
    }
    return vector;
}

QVector<Jumper *> Team::getJumpersFilteredByCountryCode(QVector<Jumper *> &jumpers, const QString &countryCode)
{
    QVector<Jumper *> vector;
    for(auto & jumper : jumpers)
    {
        if(jumper->getCountryCode().toUpper() == countryCode.toUpper())
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
    for(auto & team : *teams)
    {
        if(team.getCountryCode() == countryCode)
            return &team;
    }
    return nullptr;
}

Team Team::getTeamObjectByCountryCode(QVector<Team> *teams, QString countryCode)
{
    for(auto & team : *teams)
    {
        if(team.getCountryCode() == countryCode)
            return team;
    }
    return Team();
}

bool Team::containsTeamByCode(QVector<Team> &teams, QString code)
{
    for(auto & t : teams)
    {
        if(t.getCountryCode() == code)
            return true;
    }
    return false;
}

bool Team::containsTeamByCode(QVector<Team *> &teams, QString code)
{
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
    countryCode = newCountryCode;
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
