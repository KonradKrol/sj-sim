#ifndef HILLSLISTMODEL_H
#define HILLSLISTMODEL_H

#include <QAbstractListModel>
#include <QVector>
class Hill;

class HillsListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit HillsListModel(QVector<Hill> * hillsVectorPointer, QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QVector<Hill> * hillsVectorPointer;
public:
    QVector<Hill> *getHillsVectorPointer() const;
    void setHillsVectorPointer(QVector<Hill> *newHillsVectorPointer);
};

#endif // HILLSLISTMODEL_H
