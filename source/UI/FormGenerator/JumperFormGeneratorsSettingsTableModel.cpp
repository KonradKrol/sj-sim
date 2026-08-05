#include "JumperFormGeneratorsSettingsTableModel.h"

#include <QIcon>
#include <QFont>
#include "../../global/CountryFlagsManager.h"

JumperFormGeneratorsSettingsTableModel::JumperFormGeneratorsSettingsTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant JumperFormGeneratorsSettingsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            return tr("Zawodnik");
        case 1:
            return tr("Zmienn. tend.");
        case 2:
            return tr("Bonus tend.");
        case 3:
            return tr("Min. tend.");
        case 4:
            return tr("Maks. tend.");
        case 5:
            return tr("Wyrów. tend.");
        case 6:
            return tr("Zmienn. formy");
        case 7:
            return tr("Bonus formy");
        case 8:
            return tr("Min. forma");
        case 9:
            return tr("Maks. forma");
        }
    }
    return QVariant();
}

bool JumperFormGeneratorsSettingsTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}


int JumperFormGeneratorsSettingsTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return generatorsSettings.count();
}

int JumperFormGeneratorsSettingsTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 5;
}

QVariant JumperFormGeneratorsSettingsTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole){
        switch(index.column())
        {
        case 0:
            return generatorsSettings[index.row()].getJumper()->getNameAndSurname();
        case 1:
            return generatorsSettings[index.row()].getFormVariability();
        case 2:
            return generatorsSettings[index.row()].getFormBonus();
        case 3:
            return generatorsSettings[index.row()].getMinForm();
        case 4:
            return generatorsSettings[index.row()].getMaxForm();
        }
    }
    else if(role == Qt::DecorationRole)
    {
        if(index.column() == 0)
            return QIcon(CountryFlagsManager::getFlagPixmap(generatorsSettings[index.row()].getJumper()->getCountryCode().toLower()));
    }
    else if(role == Qt::ForegroundRole)
    {
        if(index.column() == 2)
        {
            double bonus = generatorsSettings[index.row()].getFormBonus();
            if(bonus > 0)
                return QColor(qRgb(114, 181, 107));
            else if(bonus < 0)
                return QColor(qRgb(179, 100, 96));
            else
                return QColor(qRgb(40, 40, 40));
        }
    }
    else if(role == Qt::TextAlignmentRole)
        return Qt::AlignHCenter;
    else if(role == Qt::FontRole)
    {
        QFont font("Quicksand Medium", 7);
        if(index.column() == 0)
            font.setBold(true);
        else if(index.column() == 3 || index.column() == 4)
            font.setItalic(true);
        return font;
    }

    // FIXME: Implement me!
    return QVariant();
}

bool JumperFormGeneratorsSettingsTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        switch(index.column())
        {
        case 1:
            if(value.toDouble() < 0 || value.toDouble() > 20)
                return false;
            generatorsSettings[index.row()].setFormVariability(value.toDouble());
            break;
        case 2:
            if(value.toDouble() < -100 || value.toDouble() > 100)
                return false;
            generatorsSettings[index.row()].setFormBonus(value.toDouble());
            break;
        case 3:
            if(value.toDouble() < 0 || value.toDouble() > 100)
                return false;
            generatorsSettings[index.row()].setMinForm(value.toDouble());
            break;
        case 4:
            if(value.toDouble() < 0 || value.toDouble() > 100)
                return false;
            generatorsSettings[index.row()].setMaxForm(value.toDouble());
            break;
        }

        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags JumperFormGeneratorsSettingsTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if(index.column() == 0)
        return QAbstractItemModel::flags(index);

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable; // FIXME: Implement me!
}

bool JumperFormGeneratorsSettingsTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

bool JumperFormGeneratorsSettingsTableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
    return true;
}

QVector<JumperFormGeneratorSettings> JumperFormGeneratorsSettingsTableModel::getGeneratorsSettings() const
{
    return generatorsSettings;
}

QVector<JumperFormGeneratorSettings> &JumperFormGeneratorsSettingsTableModel::getGeneratorsSettingsReference()
{
    return generatorsSettings;
}

void JumperFormGeneratorsSettingsTableModel::setGeneratorsSettings(const QVector<JumperFormGeneratorSettings> &newGeneratorsSettings)
{
    generatorsSettings = newGeneratorsSettings;
}
