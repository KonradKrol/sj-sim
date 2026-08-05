#ifndef TEAMEDITORWIDGET_H
#define TEAMEDITORWIDGET_H

#include <QWidget>
#include <QVector>
#include "../../../simulator/Team.h"
#include "../../../simulator/Jumper.h"
#include "Models/TeamJumpersListModel.h"

namespace Ui {
class TeamEditorWidget;
}

class TeamEditorWidget : public QWidget
{
    Q_OBJECT

public:
    TeamEditorWidget(Team * team, int jumpersCount = 0, QWidget * parent = nullptr);
    ~TeamEditorWidget();

    void fillWidgetInputs();
    void removeSubmitButton();
    Team constructTeamFromWidgetInput();

signals:
    void submitted();
    void needToUpdateModel();

private:
    Ui::TeamEditorWidget *ui;
    TeamJumpersListModel * model;

    Team * team;
    QVector<Jumper *> jumpers;
    int jumpersCount;

private slots:
    void when_submitButtonClicked();
    void on_pushButton_up_clicked();
    void on_pushButton_down_clicked();

public:
    Team *getTeam() const;
    void setTeam(Team *newTeam);
    QVector<Jumper *> getJumpers() const;
    void setJumpers(const QVector<Jumper *> &newJumpers);
    int getJumpersCount() const;
    void setJumpersCount(int newJumpersCount);
    TeamJumpersListModel *getModel();
};

#endif // TEAMEDITORWIDGET_H
