#include "ClassificationResultsTableModel.h"
#include "../../../global/CountryFlagsManager.h"
#include "../../../seasons/ClassificationSingleResult.h"
#include <QModelIndex>

ClassificationResultsTableModel::ClassificationResultsTableModel(Classification *classification, QObject *parent)
    : QAbstractTableModel(parent),
    classification(classification),
    rankedResults(classification?classification->getRankedResults():QVector<ClassificationSingleResult *>())
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
        if(classification != nullptr && classification->getClassificationType() == Classification::Individual){
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

    if(classification == nullptr)
        return 0;

    return rankedResults.size();
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

    if(classification == nullptr)
        return QVariant();

    if(index.row()<0 || index.row()>=rankedResults.size())return QVariant();
    const auto *result=rankedResults[index.row()];
    if(result){
        if(role == Qt::DisplayRole)
        {
            if(index.column() == 0)
                return result->getPosition();
            else if(index.column() == 1){
                if(classification->getClassificationType() == Classification::Individual)
                    return result->getJumper()->getNameAndSurname();
                else return result->getTeamCode();
            }
            else if(index.column() == 2)
                return result->getPointsSum();
        }
        else if(role == Qt::DecorationRole)
        {
            if(index.column() == 1)
            {
                QString countryCode;
                if(classification->getClassificationType() == Classification::Individual)
                    countryCode = result->getJumper()->getCountryCode();
                else
                    countryCode = result->getTeamCode();
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
    beginResetModel();classification = newClassification;
    rankedResults=classification?classification->getRankedResults():QVector<ClassificationSingleResult *>();
    endResetModel();
}
