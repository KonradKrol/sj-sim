#ifndef GENERALCLASSIFICATIONTABLEMODEL_H
#define GENERALCLASSIFICATIONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QPair>
#include "../../../../simulator/Jumper.h"
#include "../../../../simulator/Team.h"

class GeneralClassificationTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit GeneralClassificationTableModel(QObject *parent = nullptr);

    enum Type{
        Ind,
        Tm
    };

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QVector<QPair<Jumper *, double>> indResults;
    QVector<QPair<QString, double>> teamResults;
    int type;
public:
    QVector<QPair<Jumper *, double> > getIndResults() const;
    QVector<QPair<Jumper *, double> > & getIndResultsReference();
    void setIndResults(const QVector<QPair<Jumper *, double> > &newResults);
    int getType() const;
    void setType(int newType);
    QVector<QPair<QString, double> > getTeamResults() const;
    QVector<QPair<QString, double> > & getTeamResultsReference();
    void setTeamResults(const QVector<QPair<QString, double> > &newTeamResults);
};

#endif // GENERALCLASSIFICATIONTABLEMODEL_H
