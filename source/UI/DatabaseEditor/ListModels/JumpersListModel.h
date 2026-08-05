#ifndef JUMPERSLISTMODEL_H
#define JUMPERSLISTMODEL_H

#include <QAbstractListModel>
#include <QVector>
class Jumper;

class JumpersListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit JumpersListModel(QVector<Jumper> * jumpersVectorPointer, QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
private:
    QVector<Jumper> * jumpersVectorPointer;
public:
    QVector<Jumper> *getJumpersVectorPointer();
    void setJumpersVectorPointer(QVector<Jumper> *newJumpersVectorPointer);
};

#endif // JUMPERSLISTMODEL_H
