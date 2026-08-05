#ifndef SEASONJUMPERSLISTMODEL_H
#define SEASONJUMPERSLISTMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "../../../simulator/Jumper.h"

class SeasonJumpersListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SeasonJumpersListModel(QVector<Jumper *> * seasonJumpers, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QVector<Jumper *> * seasonJumpers;
    bool showItemsNumbers;
public:
    QVector<Jumper *> *getSeasonJumpers() const;
    void setSeasonJumpers(QVector<Jumper *> *newSeasonJumpers);
    bool getShowItemsNumbers() const;
    void setShowItemsNumbers(bool newShowItemsNumbers);
};

#endif // SEASONJUMPERSLISTMODEL_H
