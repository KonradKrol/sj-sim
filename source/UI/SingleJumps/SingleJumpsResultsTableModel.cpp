#include "SingleJumpsResultsTableModel.h"

#include <QFont>

SingleJumpsResultsTableModel::SingleJumpsResultsTableModel(SingleJumpsManager *manager, QObject *parent)
    : QAbstractTableModel(parent), manager(manager)
{
}

QVariant SingleJumpsResultsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal){
        if(role == Qt::DisplayRole){
            switch(section){
            case 0:
                return tr("Lp");
            case 1:
                return tr("Odległość");
            case 2:
                return tr("Punkty");
            case 3:
                return tr("Wiatr");
            case 4:
                return tr("Lądowanie");
            }
        }
        else if(role == Qt::ForegroundRole)
            return QColor(qRgb(0, 0, 0));
    }
    return QVariant();
}

int SingleJumpsResultsTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return manager->getJumpsReference().count();
}

int SingleJumpsResultsTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 5;
}

QVariant SingleJumpsResultsTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    switch(role){
    case Qt::DisplayRole:
    {
        switch(index.column()){
        case 0:
            return index.row() + 1;
        case 1:
            return QString::number(manager->getJumpsReference()[index.row()].getDistance()) + tr("m");
        case 2:
            return QString::number(manager->getJumpsReference()[index.row()].getPoints()) + tr("pkt");
        case 3:
            return QString::number(manager->getJumpsReference()[index.row()].getAveragedWind()) + tr("m/s");
        case 4:
            switch(manager->getJumpsReference()[index.row()].getLanding().getType()){
            case Landing::TelemarkLanding:
                return tr("T");
            case Landing::BothLegsLanding:
                return tr("DN");
            case Landing::SupportLanding:
                return tr("P");
            case Landing::Fall:
                return tr("U");
            default:
                break;
            }
        }
        break;
    }
    case Qt::FontRole:{
        if(index.column() != 1)
            return QFont("Quicksand Medium", 12);
        else
            return QFont("Quicksand Medium", 13);
    }
    case Qt::ForegroundRole:{
        switch(index.column())
        {
        case 0:
            return QColor(qRgb(35, 35, 35));
        case 1:
            return QColor(qRgb(0, 0, 0));
        case 2:
            return QColor(qRgb(60, 60, 60));
        case 3:{
            double wind = manager->getJumpsReference()[index.row()].getAveragedWind();
            if(wind < 0)
                return QColor(qRgb(255, 108, 92));
            else if(wind > 0)
                return QColor(qRgb(90, 163, 82));
            else
                return QColor(qRgb(255, 255, 255));
        }
        case 4:
            switch(manager->getJumpsReference()[index.row()].getLanding().getType()){
            case Landing::TelemarkLanding:
                return QColor(qRgb(96, 196, 96));
            case Landing::BothLegsLanding:
                return QColor(qRgb(212, 200, 110));
            case Landing::SupportLanding:
                return QColor(qRgb(214, 154, 96));
            case Landing::Fall:
                return QColor(qRgb(201, 98, 89));
            default:
                break;
            }
        default:
            break;
        }
    }
    default:
        break;
    }

    return QVariant();
}
