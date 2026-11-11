#include <climits>
#include <QSet>
#include "../../simulation-v2/JumperPotential.h"
#include "../../playground/PlaygroundModel.h"
#include "../../balance/BalanceRuntime.h"
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>
#include "KOGroup.h"
#include "../CompetitionInfo.h"
#include "../RoundInfo.h"
#include "../../global/IdentifiableObjectsStorage.h"
#include "../../global/MyRandom.h"
#include "../../utilities/functions.h"

KOGroup::KOGroup()
{

}

QJsonObject KOGroup::getJsonObject(KOGroup &group)
{
    QJsonObject object;
    object.insert("id", group.getIDStr());
    object.insert("number", group.getNumber());
    QJsonArray jumpersArray;
    for(auto & jumper : group.getJumpersReference())
        jumpersArray.push_back(jumper->getIDStr());
    object.insert("jumpers-ids", jumpersArray);

    return object;
}

KOGroup KOGroup::getFromJson(const QJsonObject &json, IdentifiableObjectsStorage * storage)
{
    KOGroup group;
    group.setID(sole::rebuild(json.value("id").toString().toStdString()));
    group.setNumber(json.value("number").toInt());
    QJsonArray jumpersArray = json.value("jumpers-ids").toArray();
    for(auto val : jumpersArray)
        group.getJumpersReference().push_back(static_cast<Jumper *>(storage->get(val.toString())));

    return group;
}

QVector<KOGroup> KOGroup::constructKOGroups(RoundInfo *roundInfo, QVector<Jumper *> *jumpers, int selectionType, CompetitionInfo * competition, std::function<double(Jumper *)> potential)
{
    QVector<KOGroup> groups;
    if(roundInfo == nullptr || jumpers == nullptr || jumpers->isEmpty()
        || roundInfo->getCountInKOGroup() <= 0)
        return groups;

    if (selectionType == 4)
        selectionType = 3;
    switch(selectionType)
    {
    case KOGroup::Classic:{ //Tak jak w TCS
        if(roundInfo->getCountInKOGroup() == 2)
        {
            int additionalGroupJumpersCount = jumpers->count() % roundInfo->getCountInKOGroup();
            int number = jumpers->count() / 2;
            for(int i=0; i<number; i++)
            {
                KOGroup group;
                group.setNumber(i+1);

                Jumper * firstJumper = jumpers->at(jumpers->count() - 1 - i); //Ten gorszy w kwalifikacjach
                Jumper * secondJumper = jumpers->at(i); //Ten lepszy w kwalifikacjach
                group.setJumpers({secondJumper, firstJumper}); //na odwrót ponieważ w jumpers zawodnicy są od najgorszego do najlepszego

                groups.push_back(group);
            }
            std::reverse(groups.begin(), groups.end());
            int nm=1;
            for(auto & g : groups){
                g.setNumber(nm);
                nm++;
            }
            if(additionalGroupJumpersCount > 0) //jest konieczność zrobienia dodatkowej grupy bo nie starcza zawodniów
            {
                KOGroup additionalGroup;
                if(groups.count() > 0)
                    additionalGroup.setNumber(groups.last().getNumber() + 1);
                else
                    additionalGroup.setNumber(1);
                additionalGroup.getJumpersReference().push_back(jumpers->at(number));
                groups.push_back(additionalGroup);
            }
        }
        break;
    }
    case KOGroup::ForLargerGroups:
    case KOGroup::Interleaved:
    {
        const int size=roundInfo->getCountInKOGroup();
        const int count=(jumpers->size()+size-1)/size;
        groups.resize(count);
        for(int rank=0;rank<jumpers->size();++rank) {
            const int band=rank/count, offset=rank%count;
            const int group=selectionType==KOGroup::Interleaved && band%2?count-1-offset:offset;
            groups[group].getJumpersReference().prepend(jumpers->at(jumpers->size()-1-rank));
        }
        std::reverse(groups.begin(),groups.end());
        for(int i=0;i<groups.size();++i)groups[i].setNumber(i+1);
        break;
    }
    case KOGroup::BalancedPotential:
    {
        const int size=roundInfo->getCountInKOGroup(), count=(jumpers->size()+size-1)/size;
        groups.resize(count);QVector<double> sum(count),squares(count);QVector<int> capacity(count,jumpers->size()/count);
        for(int i=0;i<jumpers->size()%count;++i)++capacity[i];
        QHash<Jumper *,double> values;
        const auto parameters=Playground::engineParameters(competition && competition->simulationBalance?*competition->simulationBalance:BalanceRuntime::gameCapture());
        for(auto *j:*jumpers) {
            double value=0;
            if(potential)value=potential(j);
            else if(competition && competition->getHill())value=SimulationV2::jumperPotential(*j,*competition->getHill(),parameters,nullptr,0,SimulationV2::EventKind::Competition);
            else if(auto profile=j->getSimulationProfile())value=profile->overall+(profile->form-50)*.3;
            values[j]=std::isfinite(value)?value:0;
        }
        auto ordered=*jumpers;std::stable_sort(ordered.begin(),ordered.end(),[&](auto *a,auto *b){return values[a]>values[b];});
        double total=0,totalSquares=0;
        for(auto *j:ordered) {
            int best=-1;
            for(int i=0;i<count;++i)if(groups[i].getJumpersReference().size()<capacity[i] && (best<0 || sum[i]/capacity[i]<sum[best]/capacity[best]))best=i;
            const double value=values[j];groups[best].getJumpersReference().append(j);sum[best]+=value;squares[best]+=value*value;total+=value;totalSquares+=value*value;
        }
        const double mean=total/jumpers->size(),deviation=std::sqrt(std::max(0.,totalSquares/jumpers->size()-mean*mean));
        const auto cost=[&](int group,double s,double sq) {
            const double avg=s/capacity[group],spread=std::sqrt(std::max(0.,sq/capacity[group]-avg*avg));
            return 25*(avg-mean)*(avg-mean)+(spread-deviation)*(spread-deviation);
        };
        for(int pass=0;pass<30;++pass) {
            double bestGain=1e-9;int ga=-1,gb=-1,ia=-1,ib=-1;
            for(int a=0;a<count;++a)for(int b=a+1;b<count;++b) {
                const auto &left=groups[a].getJumpersReference(),&right=groups[b].getJumpersReference();
                const double before=cost(a,sum[a],squares[a])+cost(b,sum[b],squares[b]);
                for(int i=0;i<left.size();++i)for(int j=0;j<right.size();++j) {
                    const double x=values[left[i]],y=values[right[j]],delta=y-x,squareDelta=y*y-x*x;
                    const double gain=before-cost(a,sum[a]+delta,squares[a]+squareDelta)-cost(b,sum[b]-delta,squares[b]-squareDelta);
                    if(gain>bestGain){bestGain=gain;ga=a;gb=b;ia=i;ib=j;}
                }
            }
            if(ga<0)break;
            auto &a=groups[ga].getJumpersReference(),&b=groups[gb].getJumpersReference();
            const double x=values[a[ia]],y=values[b[ib]];
            sum[ga]+=y-x;sum[gb]+=x-y;squares[ga]+=y*y-x*x;squares[gb]+=x*x-y*y;std::swap(a[ia],b[ib]);
        }
        for(int i=0;i<count;++i){groups[i].setNumber(i+1);auto &members=groups[i].getJumpersReference();std::stable_sort(members.begin(),members.end(),[&](auto *a,auto *b){return values[a]<values[b];});}
        break;
    }
    case KOGroup::BasketsNoCompatriots:
    {
        const int size=roundInfo->getCountInKOGroup(),count=(jumpers->size()+size-1)/size;
        groups.resize(count);for(int i=0;i<count;++i)groups[i].setNumber(i+1);
        // Edge-color baskets and countries: a color is a group. Alternating
        // paths preserve one athlete per basket and country whenever feasible.
        struct Edge {Jumper *jumper;int basket,country,color=-1;};
        QVector<Edge> edges;QHash<QString,int> occurrences,countryNodes;
        QVector<QVector<int>> basketColor(size,QVector<int>(count,-1)),countryColor;
        for(int i=0;i<jumpers->size();++i) {
            auto *j=jumpers->at(i);const auto code=j->getCountryCode();
            // Split only countries whose quota makes duplicates unavoidable.
            const auto node=code+"/"+QString::number(occurrences[code]++/count);
            if(!countryNodes.contains(node)){countryNodes[node]=countryColor.size();countryColor.append(QVector<int>(count,-1));}
            edges.append({j,i/count,countryNodes[node],-1});
        }
        std::mt19937 random(std::random_device{}());
        QVector<int> processing(edges.size());std::iota(processing.begin(),processing.end(),0);std::shuffle(processing.begin(),processing.end(),random);
        for(int index:processing) {
            auto &edge=edges[index];QVector<int> colors(count);std::iota(colors.begin(),colors.end(),0);std::shuffle(colors.begin(),colors.end(),random);
            int a=-1,b=-1,common=-1;
            for(int color:colors) {
                if(basketColor[edge.basket][color]<0){a=color;if(countryColor[edge.country][color]<0){common=color;break;}}
                if(countryColor[edge.country][color]<0)b=color;
            }
            if(common>=0)a=common;
            else {
                QVector<int> path;bool atCountry=true;int vertex=edge.country,color=a;
                while(true) {
                    const int old=atCountry?countryColor[vertex][color]:basketColor[vertex][color];
                    if(old<0)break;
                    path.append(old);const auto &item=edges[old];
                    vertex=atCountry?item.basket:item.country;atCountry=!atCountry;color=color==a?b:a;
                }
                for(int old:path){const auto &item=edges[old];basketColor[item.basket][item.color]=-1;countryColor[item.country][item.color]=-1;}
                for(int old:path){auto &item=edges[old];item.color=item.color==a?b:a;basketColor[item.basket][item.color]=old;countryColor[item.country][item.color]=old;}
            }
            edge.color=a;basketColor[edge.basket][a]=index;countryColor[edge.country][a]=index;
        }
        for(const auto &edge:edges)groups[edge.color].getJumpersReference().append(edge.jumper);
        break;
    }
    case KOGroup::BasketsDraw:
    {
        QVector<QVector<Jumper *>> baskets;
        const int groupSize = roundInfo->getCountInKOGroup();
        const int groupsCount = (jumpers->count() + groupSize - 1) / groupSize;

        for(int basketIndex = 0; basketIndex < groupSize; ++basketIndex)
        {
            const int from = basketIndex * groupsCount;
            const int to = std::min(from + groupsCount, static_cast<int>(jumpers->count()));
            QVector<Jumper *> basket;
            for(int index = from; index < to; ++index)
                basket.push_back(jumpers->at(index));
            if(!basket.isEmpty())
                baskets.push_back(basket);
        }

        //Losowanie
        for(int i=0; i<groupsCount; i++)
        {
            KOGroup group;
            group.setNumber(i+1);
            //group.setCompetitionType(competition->getRulesPointer()->getCompetitionType());

            groups.push_back(group);
        }
        for(auto basket : baskets)
        {
            QVector<int> availableGroups;
            availableGroups.reserve(groups.count());
            for(int groupIndex = 0; groupIndex < groups.count(); ++groupIndex)
                availableGroups.push_back(groupIndex);

            while(!basket.isEmpty())
            {
                const int randomJumperIndex = MyRandom::randomInt(0, basket.count() - 1);
                const int randomGroupListIndex = MyRandom::randomInt(0, availableGroups.count() - 1);
                groups[availableGroups.at(randomGroupListIndex)].getJumpersReference().push_back(basket.at(randomJumperIndex));
                basket.removeAt(randomJumperIndex);
                availableGroups.removeAt(randomGroupListIndex);
            }
        }
        qDebug()<<"groups count: "<<groups.count();
            if(groups.count() > 0)
        qDebug()<<"first "<<groups.first().getJumpersReference().count();
        break;
    }
    case KOGroup::Random: {
        int groupsCount = jumpers->count() / roundInfo->getCountInKOGroup();
        for(int i=0; i<groupsCount; i++)
        {
            KOGroup group;
            group.setNumber(i+1);

            groups.push_back(group);
        }
        QVector<Jumper *> toDraw = *jumpers;
        toDraw.detach();
        int normalJumpersCountLimit = roundInfo->getCountInKOGroup();

        for(auto & group : groups)
        {
            if(group.getJumpersReference().count() == normalJumpersCountLimit)
                continue;
            else
            {
                for(int i=0; i<roundInfo->getCountInKOGroup(); i++){
                    if(toDraw.count() > 0){
                        Jumper * randomJumper = toDraw.at(MyRandom::randomInt(0, toDraw.count() - 1));
                        group.getJumpersReference().push_back(randomJumper);
                        MyFunctions::removeFromVector(toDraw, randomJumper);
                    }
                }
            }
        }
        if(toDraw.count() > 0)
        {
            KOGroup additionalGroup;
            if(groups.count() > 0)
                additionalGroup.setNumber(groups.last().getNumber() + 1);
            else
                additionalGroup.setNumber(1);

            for(auto & j : toDraw)
                additionalGroup.getJumpersReference().push_back(j);
            groups.push_back(additionalGroup);
        }
        break;
    }
    }
    return groups;
}

QVector<Jumper *> KOGroup::getJumpersFromGroups(QVector<KOGroup> *groups)
{
    QVector<Jumper *> jumpers;
    for(auto & group : *groups)
    {
        for(auto & jumper : group.getJumpersReference())
            jumpers.push_back(jumper);
    }
    return jumpers;
}

QVector<Jumper *> KOGroup::getJumpers() const
{
    return jumpers;
}

QVector<Jumper *> &KOGroup::getJumpersReference()
{
    return jumpers;
}

void KOGroup::setJumpers(const QVector<Jumper *> &newJumpers)
{
    jumpers = newJumpers;
}

int KOGroup::getNumber() const
{
    return number;
}

void KOGroup::setNumber(int newNumber)
{
    number = newNumber;
}
