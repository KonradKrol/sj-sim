#include "CountriesEditorTableModel.h"
#include "../../global/CountryFlagsManager.h"
#include <QIcon>
#include <QFont>

CountriesEditorTableModel::CountriesEditorTableModel(QObject *parent)
    : QAbstractTableModel(parent),
    countries(&GlobalDatabase::get()->getCountriesReference())
{
}

QVariant CountriesEditorTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole || orientation == Qt::Vertical)
    {
        switch(section)
        {
        case 0:
            return tr("Kod alpha3");
        case 1:
            return tr("Kod alpha2");
        case 2:
            return tr("Nazwa kraju");
        }
    }
    return QVariant();
}

bool CountriesEditorTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}


int CountriesEditorTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return countries->count();
}

int CountriesEditorTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 3;
}

QVariant CountriesEditorTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        if(index.column() == 0)
            return countries->at(index.row()).getAlpha3();
        else if(index.column() == 1)
            return countries->at(index.row()).getAlpha2();
        else if(index.column() == 2)
            return countries->at(index.row()).getName();
    }
    else if(role == Qt::DecorationRole)
    {
        if(index.column() == 0)
        {
            return QIcon(CountryFlagsManager::getFlagPixmap(countries->at(index.row()).getAlpha3()));
        }
    }
    else if(role == Qt::FontRole)
    {
        QFont font("Quicksand Medium", 10);
    }
    else if(role == Qt::TextAlignmentRole)
        return Qt::AlignHCenter;

    // FIXME: Implement me!
    return QVariant();
}

bool CountriesEditorTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value && value.toString() != "") {
        // FIXME: Implement me!
        if(index.column() == 0 && value.toString().count() > 3)
            return false;
        if(index.column() == 1 && value.toString().count() > 2)
            return false;

        if(index.column() == 0)
            (*countries)[index.row()].setAlpha3(value.toString());
        else if(index.column() == 1)
            (*countries)[index.row()].setAlpha2(value.toString());
        else if(index.column() == 2)
            (*countries)[index.row()].setName(value.toString());
        emit edited();

        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags CountriesEditorTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable; // FIXME: Implement me!
}

bool CountriesEditorTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool CountriesEditorTableModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
    return true;
}

bool CountriesEditorTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

bool CountriesEditorTableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
    return true;
}

QVector<Country> *CountriesEditorTableModel::getCountries() const
{
    return countries;
}

void CountriesEditorTableModel::setCountries(QVector<Country> *newCountries)
{
    countries = newCountries;
}
