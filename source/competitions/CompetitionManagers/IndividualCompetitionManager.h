#ifndef INDIVIDUALCOMPETITIONMANAGER_H
#define INDIVIDUALCOMPETITIONMANAGER_H

#include <QVector>
#include <QObject>

#include "../AbstractCompetitionManager.h"
#include "../../simulator/Jumper.h"
#include "../CompetitionSingleResult.h"
#include "../CompetitionResults.h"
#include "../CompetitionInfo.h"
#include "KORoundManager.h"
#include "../../seasons/SaveJumpersList.h"

class IndividualCompetitionManager : public AbstractCompetitionManager
{
    Q_OBJECT
public:
    IndividualCompetitionManager();

    void setupNextRound(QVector<KOGroup> manualGroups = QVector<KOGroup>());
    void setupStartListStatusesForActualRound();
    void updateCompetitorsAdvanceStatuses();

    QVector<Jumper *> getFilteredJumpersForNextRound(bool checkKO = false);
    static QVector<Jumper *> getFilteredJumpersAfterQualifications(CompetitionInfo *competition, QVector<Jumper *> jumpers);
    static QVector<Jumper *> getFilteredJumpersByClassification(CompetitionInfo *competition, Classification * classification, QVector<Jumper *> & jumpers);

    QVector<KOGroup> getFilteredGroupsForNextRound();

    static void setStartListOrderByClassification(QVector<Jumper *> &jumpers, Classification * classification);
    static void setStartListOrderByCompetitionResults(QVector<Jumper *> &jumpers, CompetitionInfo * competition);
    static void setStartListOrderByDefault(QVector<Jumper *> * jumpersList, QVector<Jumper *> & startList);
    static void setStartListOrderRandomly(QVector<Jumper *> & startList);
    static void setStartListOrderByCountries(QVector<QString> countries, QVector<Jumper *> & startList);
    static void setStartListOrderByJumpersList(SaveJumpersList * list, QVector<Jumper *> & startList);

private:
    QVector<QVector<Jumper *>> roundsJumpers;
    QVector<QVector<KOGroup> *> roundsKOGroups;
    KORoundManager * KOManager;

public:
    QVector<Jumper *> & getFirstRoundJumpersReference() {return roundsJumpers[0];}
    QVector<Jumper *> & getActualRoundJumpersReference() {return roundsJumpers[actualRound - 1];}
    void setFirstRoundJumpers(QVector<Jumper *> jumpers);
    QVector<QVector<Jumper *> > getRoundsJumpers() const;
    QVector<QVector<Jumper *> > & getRoundsJumpersReference();
    void setRoundsJumpers(const QVector<QVector<Jumper *> > &newRoundsJumpers);
    QVector<StartListCompetitorStatus> getStartListStatuses() const;
    KORoundManager *getKOManager() const;
    void setKOManager(KORoundManager *newKOManager);
    QVector<QVector<KOGroup> *> getRoundsKOGroups() const;
    QVector<QVector<KOGroup> *> & getRoundsKOGroupsReference();
    void setRoundsKOGroups(const QVector<QVector<KOGroup> *> &newRoundsKOGroups);
};

#endif // INDIVIDUALCOMPETITIONMANAGER_H
