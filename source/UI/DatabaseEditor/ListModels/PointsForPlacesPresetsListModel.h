    #ifndef POINTSFORPLACESPRESETSLISTMODEL_H
#define POINTSFORPLACESPRESETSLISTMODEL_H

#include <QAbstractListModel>
#include <QVector>
class PointsForPlacesPreset;

class PointsForPlacesPresetsListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PointsForPlacesPresetsListModel(QVector<PointsForPlacesPreset> * presetsVectorPointer, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QVector<PointsForPlacesPreset> * presetsVectorPointer;
public:
    QVector<PointsForPlacesPreset> *getPresetsVectorPointer() const;
    void setPresetsVectorPointer(QVector<PointsForPlacesPreset> *newPresetsVectorPointer);
};

#endif // POINTSFORPLACESPRESETSLISTMODEL_H
