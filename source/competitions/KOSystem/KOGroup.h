#ifndef KOGROUP_H
#define KOGROUP_H

#include <QVector>
#include <QJsonObject>
#include "../../simulator/Jumper.h"
#include "../../simulator/Team.h"
#include "../../utilities/Identifiable.h"

class CompetitionInfo;
class CompetitionRules;
class RoundInfo;

class KOGroup : public Identifiable
{
public:
    KOGroup();

    enum GroupsSelectionType{
        Classic, //50-1, 49-2, 30-21
        ForLargerGroups, //1, 11, 21, 31, 41
        BasketsDraw,
        Random
    };

    static QJsonObject getJsonObject(KOGroup &group);
    static KOGroup getFromJson(const QJsonObject & json, IdentifiableObjectsStorage *storage);

    static QVector<KOGroup> constructKOGroups(RoundInfo * roundInfo, QVector<Jumper *> * jumpers, int selectionType, CompetitionInfo *competition);
    static QVector<Jumper *> getJumpersFromGroups(QVector<KOGroup> * groups);

private:
    int number;

    //Aktywny jeden z tych vectorów w zależności competitionType
    QVector<Jumper *> jumpers;
public:
    int getNumber() const;
    void setNumber(int newNumber);
    QVector<Jumper *> getJumpers() const;
    QVector<Jumper *> & getJumpersReference();
    void setJumpers(const QVector<Jumper *> &newJumpers);
};

#endif // KOGROUP_H
