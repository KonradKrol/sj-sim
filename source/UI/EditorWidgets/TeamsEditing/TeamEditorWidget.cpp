#include "TeamEditorWidget.h"
#include "ui_TeamEditorWidget.h"
#include <QDebug>

#include "../../../global/CountryFlagsManager.h"

TeamEditorWidget::TeamEditorWidget(Team *team, int jumpersCount, QWidget * parent) : team(team),
    ui(new Ui::TeamEditorWidget),
    QWidget(parent), jumpersCount(jumpersCount)
{
    ui->setupUi(this);

    connect(ui->pushButton_submit, &QPushButton::clicked, this, &TeamEditorWidget::when_submitButtonClicked);

    jumpers = team->getJumpers();
    model = new TeamJumpersListModel(&jumpers, jumpersCount, this);

    ui->listView_jumpers->setModel(model);
}

TeamEditorWidget::~TeamEditorWidget()
{
    delete ui;
}

void TeamEditorWidget::fillWidgetInputs()
{
    ui->label_teamCountryCode->setText(team->getCountryCode());
    ui->label_flagPixmap->setPixmap(CountryFlagsManager::getFlagPixmap(team->getCountryCode().toLower()).scaled(ui->label_flagPixmap->size()));
}

void TeamEditorWidget::removeSubmitButton()
{
    disconnect(ui->pushButton_submit, &QPushButton::clicked, this, &TeamEditorWidget::when_submitButtonClicked);
    ui->verticalLayout->removeWidget(ui->pushButton_submit);
    delete ui->pushButton_submit;
}

Team TeamEditorWidget::constructTeamFromWidgetInput()
{
    Team team;
    team.setCountryCode(this->team->getCountryCode());
    team.setJumpers(jumpers);
    return team;
}

TeamJumpersListModel *TeamEditorWidget::getModel()
{
    return model;
}

int TeamEditorWidget::getJumpersCount() const
{
    return jumpersCount;
}

void TeamEditorWidget::setJumpersCount(int newJumpersCount)
{
    jumpersCount = newJumpersCount;
}

QVector<Jumper *> TeamEditorWidget::getJumpers() const
{
    return jumpers;
}

void TeamEditorWidget::setJumpers(const QVector<Jumper *> &newJumpers)
{
    jumpers = newJumpers;
}

Team *TeamEditorWidget::getTeam() const
{
    return team;
}

void TeamEditorWidget::setTeam(Team *newTeam)
{
    team = newTeam;
}

void TeamEditorWidget::when_submitButtonClicked()
{
    emit submitted();
}

void TeamEditorWidget::on_pushButton_up_clicked()
{
    int index = -1;
    if(ui->listView_jumpers->selectionModel()->selectedRows().size() > 0)
        index = ui->listView_jumpers->selectionModel()->selectedRows().first().row();

    if(index > (0) && index < jumpers.count())
    {
        jumpers.swapItemsAt(index, index - 1);
        ui->listView_jumpers->clearSelection();
        ui->listView_jumpers->setCurrentIndex(model->index(index - 1, 0));
        emit model->dataChanged(model->index(index), model->index(index - 1));
    }
}


void TeamEditorWidget::on_pushButton_down_clicked()
{
    int index = -1;
    if(ui->listView_jumpers->selectionModel()->selectedRows().size() > 0)
        index = ui->listView_jumpers->selectionModel()->selectedRows().first().row();

    if(index > (-1) && (index + 1) < jumpers.count())
    {
        jumpers.swapItemsAt(index, index + 1);
        ui->listView_jumpers->clearSelection();
        ui->listView_jumpers->setCurrentIndex(model->index(index + 1, 0));
        emit model->dataChanged(model->index(index), model->index(index + 1));
    }
}

