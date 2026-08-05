#ifndef STARTLISTMODEL_H
#define STARTLISTMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "../../../simulator/Jumper.h"
#include "../../../simulator/Team.h"
#include "../../../competitions/StartListCompetitorStatus.h"
#include "../../../competitions/AbstractCompetitionManager.h"
#include "../../../competitions/KOSystem/KOGroup.h"

class StartListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit StartListModel(AbstractCompetitionManager * manager, QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QVector<StartListCompetitorStatus> * startListStatuses;
    QVector<KOGroup> * KOGroups;

    short type;
public:
    enum Type{
        IndividualCompetiton,
        TeamCompetition
    };

    short getType() const;
    void setType(short newType);
    QVector<StartListCompetitorStatus> *getStartListStatuses() const;
    void setStartListStatuses(QVector<StartListCompetitorStatus> *newStartListStatuses);
    QVector<KOGroup> *getKOGroups() const;
    void setKOGroups(QVector<KOGroup> *newKOGroups);
};

#endif // STARTLISTMODEL_H
