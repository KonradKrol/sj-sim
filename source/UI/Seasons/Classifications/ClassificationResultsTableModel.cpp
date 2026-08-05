#include "ClassificationResultsTableModel.h"
#include "../../../global/CountryFlagsManager.h"
#include "../../../seasons/ClassificationSingleResult.h"
#include <QModelIndex>

ClassificationResultsTableModel::ClassificationResultsTableModel(Classification *classification, QObject *parent)
    : QAbstractTableModel(parent),
    classification(classification)
{
}

ClassificationResultsTableModel::~ClassificationResultsTableModel()
{

}

QVariant ClassificationResultsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(section)
    {
    case 0:
        return tr("Poz.");
    case 1:
        if(classification->getClassificationType() == Classification::Individual){
            return tr("Zawodnik");
        }
        else{
            return tr("Drużyna");
        }
    case 2:
        return tr("Punkty");
    }
    return QVariant();
}

int ClassificationResultsTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return classification->getResultsReference().count();
}

int ClassificationResultsTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 3;
}

QVariant ClassificationResultsTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(classification->getResultsReference().count() > 0){
        if(role == Qt::DisplayRole)
        {
            if(index.column() == 0)
                return classification->getResultsReference()[index.row()]->getPosition();
            else if(index.column() == 1){
                if(classification->getClassificationType() == Classification::Individual)
                    return classification->getResultsReference()[index.row()]->getJumper()->getNameAndSurname();
                else return classification->getResultsReference()[index.row()]->getTeamCode();
            }
            else if(index.column() == 2)
                return classification->getResultsReference()[index.row()]->getPointsSum();
        }
        else if(role == Qt::DecorationRole)
        {
            if(index.column() == 1)
            {
                QString countryCode;
                if(classification->getClassificationType() == Classification::Individual)
                    countryCode = classification->getResultsReference()[index.row()]->getJumper()->getCountryCode();
                else
                    countryCode = classification->getResultsReference()[index.row()]->getTeamCode();
                return CountryFlagsManager::getFlagPixmap(countryCode.toLower()).scaled(28, 19.95);
            }
        }
        else if(role == Qt::FontRole)
        {
            QFont font("Ubuntu", 10);
            if(index.column() == 0)
                font.setBold(true);
            else if(index.column() == 2)
                font.setBold(true);
            return font;
        }
        else if(role == Qt::BackgroundRole)
        {
            return QColor(qRgb(255, 255, 255));
        }
        else if(role == Qt::ForegroundRole)
        {
            return QColor("black");
        }
        else if(role == Qt::TextAlignmentRole)
            return Qt::AlignHCenter;
    }

    return QVariant();
}

bool ClassificationResultsTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool ClassificationResultsTableModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
    return true;
}

bool ClassificationResultsTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

bool ClassificationResultsTableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
    return true;
}

Classification *ClassificationResultsTableModel::getClassification() const
{
    return classification;
}

void ClassificationResultsTableModel::setClassification(Classification *newClassification)
{
    classification = newClassification;
}
