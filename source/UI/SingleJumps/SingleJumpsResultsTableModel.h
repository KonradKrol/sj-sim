#ifndef SINGLEJUMPSRESULTSTABLEMODEL_H
#define SINGLEJUMPSRESULTSTABLEMODEL_H

#include <QAbstractTableModel>
#include "../../single-jumps/SingleJumpsManager.h"

class SingleJumpsResultsTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit SingleJumpsResultsTableModel(SingleJumpsManager * manager, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    SingleJumpsManager * manager;
};

#endif // SINGLEJUMPSRESULTSTABLEMODEL_H
