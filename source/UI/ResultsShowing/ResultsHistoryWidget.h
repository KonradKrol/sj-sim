#pragma once
#include <QWidget>
#include <QPointer>
#include <QStandardItemModel>
#include "../../competitions/CompetitionResults.h"
class QComboBox;
class QTreeView;
class JumperCompetitionResultsWidget;

class ResultsHistoryWidget : public QWidget {
    Q_OBJECT
public:
    ResultsHistoryWidget(CompetitionInfo *competition,bool live,QWidget *parent);
    void refresh(int completedRounds=-1);
    bool active() const;
    bool paused() const;
    bool individualRanking() const;
    QComboBox *roundChoice() const {return rounds;}
    QComboBox *modeChoice() const {return mode;}
    QTreeView *tableView() const {return table;}
    JumperCompetitionResultsWidget *previewWidget() const {return preview;}
    CompetitionResults snapshot;
    static CompetitionResults afterRound(const CompetitionResults &source,int round,bool individual,CompetitionInfo *owner=nullptr);
signals:
    void changed();
private:
    CompetitionInfo *competition;
    bool live;
    QPointer<QWidget> owner;
    QComboBox *rounds,*mode;
    QTreeView *table;
    QStandardItemModel *model;
    JumperCompetitionResultsWidget *preview;
    QString signature;
    int available=0;
    int completedLimit=-1;
    bool wasPaused=false;
    QVector<QPair<QPointer<QWidget>,bool>> disabledControls;
    void applyPause();
    void rebuild();
    void showJump(const QModelIndex &index);
};
