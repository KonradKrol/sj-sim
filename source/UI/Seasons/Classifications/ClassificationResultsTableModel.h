#ifndef CLASSIFICATIONRESULTSTABLEMODEL_H
#define CLASSIFICATIONRESULTSTABLEMODEL_H

#include <QAbstractTableModel>
#include "../../../seasons/Classification.h"

class ClassificationResultsTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ClassificationResultsTableModel(Classification * classification = nullptr, QObject *parent = nullptr);
    ~ClassificationResultsTableModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    Classification * classification;
public:
    Classification *getClassification() const;
    void setClassification(Classification *newClassification);
};

#endif // CLASSIFICATIONRESULTSTABLEMODEL_H
