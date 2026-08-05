#ifndef CALENDAREDITORTABLEMODEL_H
#define CALENDAREDITORTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "../../../seasons/SeasonCalendar.h"
#include "../../../simulator/Hill.h"
#include "../../../competitions/CompetitionRules.h"

class CalendarEditorTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CalendarEditorTableModel(SeasonCalendar * calendar, QVector<Hill *> *hillsList, QVector<CompetitionRules> * rulesList, int dontModifiyBefore = 0, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    SeasonCalendar * calendar;
    QVector<Hill *> * hillsList;
    QVector<CompetitionRules> * rulesList;
    int dontModifyBefore;
public:
    SeasonCalendar *getCalendar() const;
    void setCalendar(SeasonCalendar *newCalendar);
    QVector<Hill *> *getHillsList() const;
    void setHillsList(QVector<Hill *> *newHillsList);
    QVector<CompetitionRules> *getRulesList() const;
    void setRulesList(QVector<CompetitionRules> *newRulesList);
    int getDontModifyBefore() const;
    void setDontModifyBefore(int newDontModifiyBefore);
};

#endif // CALENDAREDITORTABLEMODEL_H
