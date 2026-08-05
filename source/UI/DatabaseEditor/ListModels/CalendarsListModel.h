#ifndef CALENDARSLISTMODEL_H
#define CALENDARSLISTMODEL_H

#include <QAbstractListModel>
#include "../../../seasons/SeasonCalendar.h"

class CalendarsListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit CalendarsListModel(QVector<SeasonCalendar *> * calendars, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QVector<SeasonCalendar *> * calendars;
public:
    QVector<SeasonCalendar *> *getCalendars() const;
    void setCalendars(QVector<SeasonCalendar *> *newCalendars);
};

#endif // CALENDARSLISTMODEL_H
