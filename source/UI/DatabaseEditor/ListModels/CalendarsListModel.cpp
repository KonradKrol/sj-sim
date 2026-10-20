#include "CalendarsListModel.h"

CalendarsListModel::CalendarsListModel(QVector<SeasonCalendar *> *calendars, QObject *parent)
    : QAbstractListModel(parent),
    calendars(calendars)
{
}

QVariant CalendarsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    return QVariant();
}

int CalendarsListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return calendars->count();
}

QVariant CalendarsListModel::data(const QModelIndex &index, int role) const
{
    if (!calendars || !index.isValid() || index.row()<0 || index.row()>=calendars->size())
        return QVariant();

    if(role == Qt::DisplayRole){
        return calendars->at(index.row())->getName();
    }
    else if(role == Qt::FontRole){
        return QFont("Quicksand Medium", 11, 600);
    }
    else if(role == Qt::BackgroundRole){
        return QColor(qRgb(253, 253, 253));
    }
    else if(role == Qt::ForegroundRole){
        return QColor(qRgb(30, 30, 30));
    }

    // FIXME: Implement me!
    return QVariant();
}

bool CalendarsListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(parent.isValid() || !calendars || row<0 || row>calendars->size() || count<=0)
        return false;
    beginInsertRows(parent, row, row + count - 1);
    for(int i=0;i<count;++i)calendars->insert(row+i,new SeasonCalendar(tr("Kalendarz")));
    endInsertRows();
    return true;
}

bool CalendarsListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(parent.isValid() || !calendars || row<0 || count<=0 || row>calendars->size()-count)
        return false;
    beginRemoveRows(parent, row, row + count - 1);
    calendars->remove(row,count);
    endRemoveRows();
    return true;
}

QVector<SeasonCalendar *> *CalendarsListModel::getCalendars() const
{
    return calendars;
}

void CalendarsListModel::setCalendars(QVector<SeasonCalendar *> *newCalendars)
{
    calendars = newCalendars;
}
