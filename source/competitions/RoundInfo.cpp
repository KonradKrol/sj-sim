#include "RoundInfo.h"

RoundInfo::RoundInfo(int count, bool sortStartList) : count(count),
    sortStartList(sortStartList)
{}

int RoundInfo::getKoGroupsSelectionType() const
{
    return koGroupsSelectionType;
}

void RoundInfo::setKoGroupsSelectionType(int newKoGroupsSelectionType)
{
    koGroupsSelectionType = newKoGroupsSelectionType;
}

int RoundInfo::getCountInKOGroup() const
{
    return countInKOGroup;
}

void RoundInfo::setCountInKOGroup(int newCountInKOGroup)
{
    countInKOGroup = newCountInKOGroup;
}

int RoundInfo::getAdvancingFromKOGroup() const
{
    return advancingFromKOGroup;
}

void RoundInfo::setAdvancingFromKOGroup(int newAdvancingFromKOGroup)
{
    advancingFromKOGroup = newAdvancingFromKOGroup;
}

bool RoundInfo::getKO() const
{
    return KO;
}

void RoundInfo::setKO(bool newKO)
{
    KO = newKO;
}

int RoundInfo::getSortAfterGroups() const
{
    return sortAfterGroups;
}

void RoundInfo::setSortAfterGroups(int newSortAfterGroups)
{
    sortAfterGroups = newSortAfterGroups;
}

int RoundInfo::getCount() const
{
    return count;
}

void RoundInfo::setCount(int newCount)
{
    count = newCount;
}

bool RoundInfo::getSortStartList() const
{
    return sortStartList;
}

void RoundInfo::setSortStartList(bool newSortStartList)
{
    sortStartList = newSortStartList;
}
