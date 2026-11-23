#include "CountriesEditorTableModel.h"
#include "../../global/CountryFlagsManager.h"
#include <QIcon>

CountriesEditorTableModel::CountriesEditorTableModel(QVector<Country> *countries, QObject *parent)
    : QAbstractTableModel(parent), countries(countries)
{
}

QVariant CountriesEditorTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole || orientation != Qt::Horizontal)return {};
    return section == 0 ? tr("Kraj") : tr("Kod");
}

int CountriesEditorTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : countries->size();
}

int CountriesEditorTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 2;
}

QVariant CountriesEditorTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() >= countries->size())return {};
    const Country &country = countries->at(index.row());
    if(role == Qt::DisplayRole || role == Qt::EditRole)
        return index.column() == 0 ? country.getName() : country.getAlpha3();
    if(role == Qt::DecorationRole && index.column() == 0)
        return QIcon(CountryFlagsManager::getFlagPixmap(country.getAlpha3()));
    if(role == Qt::ToolTipRole && index.column() == 0)
        return tr("Kliknij dwukrotnie, aby zmienić nazwę.");
    return {};
}

bool CountriesEditorTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid() || index.column() != 0 || role != Qt::EditRole)return false;
    const QString name = value.toString().simplified();
    if(name.isEmpty() || name == (*countries)[index.row()].getName())return false;
    (*countries)[index.row()].setName(name);
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}

Qt::ItemFlags CountriesEditorTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())return Qt::NoItemFlags;
    const Qt::ItemFlags base = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return index.column() == 0 ? base | Qt::ItemIsEditable : base;
}

void CountriesEditorTableModel::addCountry(const Country &country)
{
    beginInsertRows({}, countries->size(), countries->size());
    countries->append(country);
    endInsertRows();
}

void CountriesEditorTableModel::removeCountry(int row)
{
    if(row < 0 || row >= countries->size())return;
    beginRemoveRows({}, row, row);
    countries->remove(row);
    endRemoveRows();
}
