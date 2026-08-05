#ifndef TEAMJUMPERSLISTMODEL_H
#define TEAMJUMPERSLISTMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "../../../../simulator/Jumper.h"

class TeamJumpersListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TeamJumpersListModel(QVector<Jumper *> * jumpers, int jumpersInTeamCount, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QVector<Jumper *> * jumpers;
    int jumpersInTeamCount;
public:
    int getJumpersInTeamCount() const;
    void setJumpersInTeamCount(int newJumpersInTeamCount);
    QVector<Jumper *> *getJumpers();
    void setJumpers(QVector<Jumper *> *newJumpers);
};

#endif // TEAMJUMPERSLISTMODEL_H
