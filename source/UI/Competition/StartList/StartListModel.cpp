#include "StartListModel.h"
#include "../../../global/CountryFlagsManager.h"
#include "../../../competitions/AbstractCompetitionManager.h"
#include <QIcon>
#include <QFont>
#include <QColor>
#include <QBrush>

StartListModel::StartListModel(AbstractCompetitionManager * manager, QObject *parent)
    : QAbstractListModel(parent)
{
    KOGroups = nullptr;
}

QVariant StartListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section == 0 && orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
            return "Zawodnik";
        else if(role == Qt::FontRole)
            return QFont("Ubuntu", 9, 2);
        else if(role == Qt::BackgroundRole)
            return QBrush(QColor(qRgb(230, 230, 230)));
        else if(role == Qt::ForegroundRole)
            return QBrush(QColor(qRgb(20, 20, 20)));
    }

    return QVariant();
    // FIXME: Implement me!
}

int StartListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return startListStatuses->count();

    // FIXME: Implement me!
}

QVariant StartListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    else if(role == Qt::TextAlignmentRole){
        return Qt::AlignHCenter;
    }

    if(StartListCompetitorStatus::getStatusOfJumper(startListStatuses->at(index.row()).getJumper(), *startListStatuses) == nullptr)
    {
        if(role == Qt::ForegroundRole)
            return QBrush(QColor("black"));
        else if(role == Qt::BackgroundRole)
            return QBrush(QColor("white"));
        else if(role == Qt::FontRole)
            return QFont("Ubuntu", 11);
    }
    else if(role == Qt::ForegroundRole){
        if(startListStatuses->at(index.row()).getJumpStatus() == StartListCompetitorStatus::Finished){
            return QBrush(QColor(qRgb(77, 77, 77)));
        }
        else{
            return QBrush(QColor("black"));
        }
    }
    else if(role == Qt::BackgroundRole){
        if(KOGroups == nullptr){
            if(startListStatuses->at(index.row()).getJumpStatus() == StartListCompetitorStatus::Finished){
                return QBrush(QColor(qRgb(242, 242, 242)));
            }
            else{
                return QBrush(QColor("white"));
            }
        }
        else{
            Jumper * jumper = startListStatuses->at(index.row()).getJumper();
            for(auto & group : *KOGroups)
            {
                for(auto & groupJumper : group.getJumpers()){
                    if(groupJumper== jumper)
                    {
                        if(group.getNumber() % 2 == 0){
                            if(startListStatuses->at(index.row()).getJumpStatus() == StartListCompetitorStatus::Finished)
                                return QBrush(QColor(qRgb(230, 230, 218)));
                            else
                                return QBrush(QColor(qRgb(242, 245, 225)));
                        }
                        else{
                            if(startListStatuses->at(index.row()).getJumpStatus() == StartListCompetitorStatus::Finished)
                                return QBrush(QColor(qRgb(242, 242, 242)));
                            else
                                return QBrush(QColor("white"));
                        }
                    }
                }
            }
        }
    }
    else if(role == Qt::FontRole){
        QFont font("Ubuntu", 11);
        if(startListStatuses->at(index.row()).getJumpStatus() == StartListCompetitorStatus::Finished){
            font.setItalic(true);
        }
        return font;
    }
    qDebug()<<"index "<<index.row();
    qDebug()<<startListStatuses->at(index.row()).getJumper();
    qDebug()<<startListStatuses->at(index.row()).getJumper()->getPersonalBest();
    qDebug()<<startListStatuses->at(index.row()).getJumper()->getNameAndSurname();
    QString string = QString::number(index.row() + 1) + ". " + startListStatuses->at(index.row()).getJumper()->getNameAndSurname();
    if(role == Qt::DisplayRole){
        if(startListStatuses->at(index.row()).getJumpStatus() == StartListCompetitorStatus::Dns)
            return string + "   (Nie wystartował)";
        else if(startListStatuses->at(index.row()).getJumpStatus() == StartListCompetitorStatus::Dsq)
                   return string + "   (Dyskwalifikacja)";
        else
            return string;
    }
    else if(role == Qt::DecorationRole){
        return QIcon(CountryFlagsManager::getFlagPixmap(startListStatuses->at(index.row()).getJumper()->getCountryCode().toLower()));
    }
    //}
    return QVariant();
}

QVector<KOGroup> *StartListModel::getKOGroups() const
{
    return KOGroups;
}

void StartListModel::setKOGroups(QVector<KOGroup> *newKOGroups)
{
    KOGroups = newKOGroups;
}

QVector<StartListCompetitorStatus> *StartListModel::getStartListStatuses() const
{
    return startListStatuses;
}

void StartListModel::setStartListStatuses(QVector<StartListCompetitorStatus> *newStartListStatuses)
{
    startListStatuses = newStartListStatuses;
}

short StartListModel::getType() const
{
    return type;
}

void StartListModel::setType(short newType)
{
    type = newType;
}
