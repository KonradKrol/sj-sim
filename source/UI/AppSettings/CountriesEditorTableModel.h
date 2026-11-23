#ifndef COUNTRIESEDITORTABLEMODEL_H
#define COUNTRIESEDITORTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "../../global/GlobalDatabase.h"

// Countries in the editor: the name (editable, with its flag) and the code, which decides the
// flag and is fixed once a country exists. Works on a draft list owned by the editor window.
class CountriesEditorTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CountriesEditorTableModel(QVector<Country> *countries, QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void addCountry(const Country &country);
    void removeCountry(int row);
    QVector<Country> *getCountries() const { return countries; }

private:
    QVector<Country> *countries;
};

#endif // COUNTRIESEDITORTABLEMODEL_H
