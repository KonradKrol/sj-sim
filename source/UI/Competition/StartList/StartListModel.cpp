#include <QObject>
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
            return QObject::tr("Zawodnik");
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

    return displayedStatuses.count();

    // FIXME: Implement me!
}

QVariant StartListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column()!=0 || index.row()<0 || index.row()>=displayedStatuses.size())
        return QVariant();

    else if(role == Qt::TextAlignmentRole){
        return Qt::AlignHCenter;
    }

    if(!displayedStatuses.at(index.row()).getJumper())return {};
    if(role == Qt::ForegroundRole){
        if(displayedStatuses.at(index.row()).getJumpStatus() == StartListCompetitorStatus::Finished){
            return QBrush(QColor(qRgb(77, 77, 77)));
        }
        else{
            return QBrush(QColor("black"));
        }
    }
    else if(role == Qt::BackgroundRole){
        if(KOGroups == nullptr){
            if(displayedStatuses.at(index.row()).getJumpStatus() == StartListCompetitorStatus::Finished){
                return QBrush(QColor(qRgb(242, 242, 242)));
            }
            else{
                return QBrush(QColor("white"));
            }
        }
        else{
            Jumper * jumper = displayedStatuses.at(index.row()).getJumper();
            for(auto & group : *KOGroups)
            {
                for(auto & groupJumper : group.getJumpers()){
                    if(groupJumper== jumper)
                    {
                        if(group.getNumber() % 2 == 0){
                            if(displayedStatuses.at(index.row()).getJumpStatus() == StartListCompetitorStatus::Finished)
                                return QBrush(QColor(qRgb(230, 230, 218)));
                            else
                                return QBrush(QColor(qRgb(242, 245, 225)));
                        }
                        else{
                            if(displayedStatuses.at(index.row()).getJumpStatus() == StartListCompetitorStatus::Finished)
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
        if(displayedStatuses.at(index.row()).getJumpStatus() == StartListCompetitorStatus::Finished){
            font.setItalic(true);
        }
        return font;
    }
    QString string = QString::number(index.row() + 1) + ". " + displayedStatuses.at(index.row()).getJumper()->getNameAndSurname();
    if(role == Qt::DisplayRole){
        if(displayedStatuses.at(index.row()).getJumpStatus() == StartListCompetitorStatus::Dns)
            return string + QObject::tr("   (Nie wystartował)");
        else if(displayedStatuses.at(index.row()).getJumpStatus() == StartListCompetitorStatus::Dsq)
                   return string + "   (Dyskwalifikacja)";
        else
            return string;
    }
    else if(role == Qt::DecorationRole){
        return QIcon(CountryFlagsManager::getFlagPixmap(displayedStatuses.at(index.row()).getJumper()->getCountryCode().toLower()).scaled(QSize(24, 14)));
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
    refresh();
}

short StartListModel::getType() const
{
    return type;
}

void StartListModel::setType(short newType)
{
    type = newType;
}

void StartListModel::refresh()
{
    const auto next = startListStatuses ? *startListStatuses : QVector<StartListCompetitorStatus>();
    bool structural = next.size() != displayedStatuses.size();
    for(int i=0;!structural && i<next.size();++i)
        structural = next[i].getJumper() != displayedStatuses[i].getJumper();
    if(structural) {
        beginResetModel();
        displayedStatuses = next;
        endResetModel();
    } else {
        displayedStatuses = next;
        if(!displayedStatuses.isEmpty())emit dataChanged(index(0),index(displayedStatuses.size()-1));
    }
}
