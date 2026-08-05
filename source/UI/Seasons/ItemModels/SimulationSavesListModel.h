#ifndef SIMULATIONSAVESLISTMODEL_H
#define SIMULATIONSAVESLISTMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "../../../seasons/SimulationSave.h"

class SimulationSavesListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SimulationSavesListModel(QVector<SimulationSave *> * globalSimulationSavesVectorPointer = nullptr, QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QVector<SimulationSave *> * globalSimulationSavesVectorPointer;
public:
    QVector<SimulationSave *> *getGlobalSimulationSavesVectorPointer() const;
    void setGlobalSimulationSavesVectorPointer(QVector<SimulationSave *> *newGlobalSimulationSavesVectorPointer);
};

#endif // SIMULATIONSAVESLISTMODEL_H
