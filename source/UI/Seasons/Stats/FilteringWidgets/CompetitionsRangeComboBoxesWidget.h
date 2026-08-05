#ifndef COMPETITIONSRANGECOMBOBOXESWIDGET_H
#define COMPETITIONSRANGECOMBOBOXESWIDGET_H

#include <QWidget>
#include <QVector>
#include "../../../../competitions/CompetitionInfo.h"
#include "../../../../seasons/Season.h"

namespace Ui {
class CompetitionsRangeComboBoxesWidget;
}

class CompetitionsRangeComboBoxesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CompetitionsRangeComboBoxesWidget(QWidget *parent = nullptr);
    ~CompetitionsRangeComboBoxesWidget();

    void setupConnections();
    void setupComboBoxes();

signals:
    void changed();

private:
    Ui::CompetitionsRangeComboBoxesWidget *ui;
    QVector<Season> * seasonsList;
    QString calendarFilter;
    bool mergeCalendars;
public:
    CompetitionInfo * getCompetition(int which);
    QVector<Season> *getSeasonsList() const;
    void setSeasonsList(QVector<Season> *newSeasonsList);
    QString getCalendarFilter() const;
    void setCalendarFilter(const QString &newCalendarFilter);
    bool getMergeCalendars() const;
    void setMergeCalendars(bool newMergeCalendars);
};

#endif // COMPETITIONSRANGECOMBOBOXESWIDGET_H
