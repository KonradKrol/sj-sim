#ifndef JUMPERFORMGENERATORSSETTINGSTABLEMODEL_H
#define JUMPERFORMGENERATORSSETTINGSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "../../form-generator/JumperFormGeneratorSettings.h"

class JumperFormGeneratorsSettingsTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit JumperFormGeneratorsSettingsTableModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QVector<JumperFormGeneratorSettings> generatorsSettings;
public:
    QVector<JumperFormGeneratorSettings> getGeneratorsSettings() const;
    QVector<JumperFormGeneratorSettings> & getGeneratorsSettingsReference();
    void setGeneratorsSettings(const QVector<JumperFormGeneratorSettings> &newGeneratorsSettings);
};

#endif // JUMPERFORMGENERATORSSETTINGSTABLEMODEL_H
