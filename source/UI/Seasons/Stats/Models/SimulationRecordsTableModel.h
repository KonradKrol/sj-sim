#ifndef SIMULATIONRECORDSTABLEMODEL_H
#define SIMULATIONRECORDSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QPair>
#include <tuple>
#include "../../../../simulator/Jumper.h"
#include "../../../../competitions/CompetitionInfo.h"
#include "../../../../seasons/SimulationSave.h"

class SimulationRecordsTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit SimulationRecordsTableModel(SimulationSave * save, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QVector<QPair<JumpData *, double>> records;
    SimulationSave * save;
public:
    SimulationSave *getSave() const;
    void setSave(SimulationSave *newSave);
    QVector<QPair<JumpData *, double> > getRecords() const;
    QVector<QPair<JumpData *, double> > & getRecordsReference();
    void setRecords(const QVector<QPair<JumpData *, double> > &newRecords);
};

#endif // SIMULATIONRECORDSTABLEMODEL_H
