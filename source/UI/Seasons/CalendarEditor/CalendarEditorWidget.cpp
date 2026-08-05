#include "CalendarEditorWidget.h"
#include "ui_CalendarEditorWidget.h"
#include "../../../global/CountryFlagsManager.h"
#include "../../../global/GlobalDatabase.h"
#include "../../../utilities/functions.h"
#include "../../../seasons/SeasonCalendarPreset.h"
#include "../../EditorWidgets/HillEditorWidget.h"
#include "MultipleTrainingsEditDialog.h"
#include "MultipleTrialRoundsEditDialog.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QAction>

CalendarEditorWidget::CalendarEditorWidget(CalendarEditorTableModel *model, QVector<Classification *> *classificationsList, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalendarEditorWidget),
    calendarModel(model),
    calendar(model->getCalendar()),
    actualCompetitionIndex(-1),
    classificationsList(classificationsList)
{
    ui->setupUi(this);
    ui->tableView->setModel(this->calendarModel);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->resizeColumnsToContents();

    action_add = new QAction(this);
    action_add->setShortcut(Qt::CTRL | Qt::Key_A);
    this->addAction(action_add);
    connect(action_add, &QAction::triggered, this, &CalendarEditorWidget::addActionTriggered);

    action_remove = new QAction(this);
    action_remove->setShortcut(Qt::CTRL | Qt::Key_D);
    this->addAction(action_remove);
    connect(action_remove, &QAction::triggered, this, &CalendarEditorWidget::removeActionTriggered);

    action_up = new QAction(this);
    action_up->setShortcut(Qt::CTRL | Qt::Key_Up);
    this->addAction(action_up);
    connect(action_up, &QAction::triggered, this, &CalendarEditorWidget::upActionTriggered);

    action_down = new QAction(this);
    action_down->setShortcut(Qt::CTRL | Qt::Key_Down);
    this->addAction(action_down);
    connect(action_down, &QAction::triggered, this, &CalendarEditorWidget::downActionTriggered);

    action_edit = new QAction(this);
    action_edit->setShortcut(Qt::CTRL | Qt::Key_E);
    this->addAction(action_edit);
    connect(action_edit, &QAction::triggered, this, &CalendarEditorWidget::editActionTriggered);

    action_duplicate = new QAction(this);
    action_duplicate->setShortcut(Qt::CTRL | Qt::ALT| Qt::Key_A);
    this->addAction(action_duplicate);
    connect(action_duplicate, &QAction::triggered, this, &CalendarEditorWidget::duplicateActionTriggered);

    defaultHill = calendarModel->getHillsList()->isEmpty()
        ? nullptr
        : calendarModel->getHillsList()->first();

    competitionInfoEditor = new CompetitionInfoEditorWidget();
    competitionInfoEditor->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    competitionInfoEditor->setParent(this);
    competitionInfoEditor->setHillsList(model->getHillsList());
    competitionInfoEditor->setupHillsComboBox();
    competitionInfoEditor->setRulesList(model->getRulesList());
    competitionInfoEditor->setupRulesComboBox();
    competitionInfoEditor->setDefaultHill(defaultHill);
    competitionInfoEditor->hide();
    connect(competitionInfoEditor, &CompetitionInfoEditorWidget::submitted, this, [this](){
        if(actualCompetitionIndex > (-1)){
            qDebug()<<"Przed usuwaniem:";
            debugCalendar();
            updateActualCompetitionByID();
            calendar->getCompetitionsReference().reserve(calendar->getCompetitionsReference().size() + competitionInfoEditor->getTrainingsCount() + 3);
            if(actualCompetition->getTrialRound() != nullptr){
                calendar->getCompetitionsReference().remove(actualCompetitionIndex - 1);
                delete actualCompetition->getTrialRound();
            }
            for(auto & training : actualCompetition->getTrainingsReference()){
                MyFunctions::removeFromVector(calendar->getCompetitionsReference(), training);
                delete training;
            }
            updateActualCompetitionByID();

            qDebug()<<"Przed dodawaniem:";
            debugCalendar();
            CompetitionInfo * competition = new CompetitionInfo(competitionInfoEditor->getCompetitionInfoFromInputs());
            if(competitionInfoEditor->getTrialRoundChecked()){
                CompetitionInfo * trialRound = new CompetitionInfo(competition->getHill());
                trialRound->setRules(competitionInfoEditor->getTrialRoundRules());
                trialRound->setSerieType(CompetitionInfo::TrialRound);
                calendar->getCompetitionsReference().insert(actualCompetitionIndex, trialRound);
                updateActualCompetitionByID();
                competition->setTrialRound(calendar->getCompetitionsReference()[actualCompetitionIndex - 1]);
            }
            for(int i=0; i<competitionInfoEditor->getTrainingsCount(); i++){
                CompetitionInfo * training = new CompetitionInfo(competition->getHill());
                training->setRules(competitionInfoEditor->getTrainingsRules());
                training->setSerieType(CompetitionInfo::Training);
                if(competitionInfoEditor->getTrialRoundChecked()){
                    calendar->getCompetitionsReference().insert(actualCompetitionIndex-1, training);
                    updateActualCompetitionByID();
                    competition->getTrainingsReference().push_back(calendar->getCompetitionsReference()[actualCompetitionIndex - 2]);
                }
                else{
                    calendar->getCompetitionsReference().insert(actualCompetitionIndex, training);
                    updateActualCompetitionByID();
                    competition->getTrainingsReference().push_back(calendar->getCompetitionsReference()[actualCompetitionIndex - 1]);
                }
            }
            qDebug()<<"Po dodawaniu:";
            debugCalendar();
            delete calendar->getCompetitionsReference()[actualCompetitionIndex];
            calendar->getCompetitionsReference()[actualCompetitionIndex] = competition;
            actualCompetitionID = competition->getIDStr();
            emit calendarModel->dataChanged(calendarModel->index(0, 0), calendarModel->index(calendarModel->rowCount() - 1, calendarModel->columnCount() - 1));
            ui->tableView->resizeColumnsToContents();
        }
    });
}

CalendarEditorWidget::~CalendarEditorWidget()
{
    delete ui;
}

void CalendarEditorWidget::updateTable()
{
    ui->tableView->setModel(nullptr);
    ui->tableView->setModel(calendarModel);
    ui->tableView->resizeColumnsToContents();
    updateActualCompetitionByID();
}

SimulationSave *CalendarEditorWidget::getSave() const
{
    return save;
}

void CalendarEditorWidget::setSave(SimulationSave *newSave)
{
    save = newSave;
    calendar = save->getActualSeason()->getActualCalendar();
}

QVector<Classification *> *CalendarEditorWidget::getClassificationsList() const
{
    return classificationsList;
}

void CalendarEditorWidget::setClassificationsList(QVector<Classification *> *newClassificationsList)
{
    classificationsList = newClassificationsList;
}

void CalendarEditorWidget::updateActualCompetitionByID()
{
    if(actualCompetitionID.isEmpty() == false){
        int i=0;
        for(auto & comp : calendar->getCompetitionsReference()){
            if(comp->getIDStr() == actualCompetitionID){
                actualCompetitionIndex = i;
                actualCompetition = comp;
                break;
            }
            i++;
        }
    }
    else{
        actualCompetition = nullptr;
        actualCompetitionIndex = (-1);
    }
}

void CalendarEditorWidget::debugCalendar()
{
    int i=0;
    for(auto & comp : calendar->getCompetitionsReference()){
        QString string = QString::number(i);
        string += ". " + comp->getHill()->getName() + " HS" + QString::number(comp->getHill()->getHSPoint()) + " -> ";
        switch(comp->getSerieType()){
        case CompetitionInfo::Competition:
            string += "KONKURS"; break;
        case CompetitionInfo::Qualifications:
            string += "KWALIFIKACJE"; break;
        case CompetitionInfo::TrialRound:
            string += "Seria próbna"; break;
            case CompetitionInfo::Training:
            string += "Trening"; break;
        default: break;
        }

        qDebug()<<string<<" ("<<comp<<") (ID: "<<comp->getID()<<")";
        i++;
    }
}

CalendarEditorTableModel *CalendarEditorWidget::getModel() const
{
    return calendarModel;
}

void CalendarEditorWidget::setModel(CalendarEditorTableModel *newModel)
{
    calendarModel = newModel;
}

QTableView *CalendarEditorWidget::getTableView()
{
    return ui->tableView;
}

/*void CalendarEditorWidget::duplicateActionTriggered()
{
    int insertIndex = 0;
    if(ui->tableView->selectionModel()->selectedIndexes().count() > 0)
        insertIndex = ui->tableView->selectionModel()->selectedIndexes().at(0).row() + 0;
    else if(calendarModel->rowCount() > 0)
        insertIndex = calendarModel->rowCount() - 1;
    else
        insertIndex = 0;

    int insertIndexInCalendar=0;
    int i=0;
    for(auto & comp : calendar->getCompetitionsReference())
    {
        if(i == insertIndex){
            break;
        }
        if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition)
            i++;
        insertIndexInCalendar++;
    }
    if(insertIndexInCalendar >= calendarModel->getDontModifyBefore()){
        calendarModel->insertRow(insertIndex);
        CompetitionInfo * selectedCompetition = SeasonCalendar::getMainCompetitionByIndex(calendar->getCompetitionsReference(), insertIndex);
        CompetitionInfo * competition = new CompetitionInfo(*selectedCompetition);
        competition->generateID();
        competition->getResultsReference().generateID();
        competition->getClassificationsReference().detach();
        competition->getQualifyingCompetitionsReference().clear();
        competition->getResultsReference().getResultsReference().clear();
        competition->getRoundsKOGroupsReference().clear();
        competition->getTeamsReference().clear();
        competition->getTrainingsReference().clear;

        calendar->getCompetitionsReference().insert(insertIndexInCalendar, competition);

        ui->tableView->clearSelection();
        ui->tableView->selectionModel()->select(calendarModel->index(insertIndex + 1, 0), QItemSelectionModel::Select);
        emit calendarModel->dataChanged(calendarModel->index(0, 0), calendarModel->index(calendarModel->rowCount() - 1, calendarModel->columnCount() - 1));
        ui->tableView->resizeColumnsToContents();
        updateActualCompetitionByID();
    }
    emit changed();
}*/

void CalendarEditorWidget::addActionTriggered()
{
    if(defaultHill == nullptr)
    {
        QMessageBox::warning(this, tr("Brak skoczni"),
                             tr("Najpierw dodaj co najmniej jedną skocznię do zapisu symulacji."), QMessageBox::Ok);
        return;
    }

    int insertIndex = 0;
    if(ui->tableView->selectionModel()->selectedIndexes().count() > 0)
        insertIndex = ui->tableView->selectionModel()->selectedIndexes().at(0).row() + 0;
    else if(calendarModel->rowCount() > 0)
        insertIndex = calendarModel->rowCount() - 1;
    else
        insertIndex = 0;

    int insertIndexInCalendar=0;
    int i=0;
    for(auto & comp : calendar->getCompetitionsReference())
    {
        if(i == insertIndex){
            break;
        }
        if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition)
            i++;
        insertIndexInCalendar++;
    }
    if(insertIndexInCalendar >= calendarModel->getDontModifyBefore()){
        calendarModel->insertRow(insertIndex);
        calendar->getCompetitionsReference().insert(insertIndexInCalendar, new CompetitionInfo(defaultHill));

        ui->tableView->clearSelection();
        ui->tableView->selectionModel()->select(calendarModel->index(insertIndex + 1, 0), QItemSelectionModel::Select);
        emit calendarModel->dataChanged(calendarModel->index(0, 0), calendarModel->index(calendarModel->rowCount() - 1, calendarModel->columnCount() - 1));
        ui->tableView->resizeColumnsToContents();
        updateActualCompetitionByID();
    }
    emit changed();
}

void CalendarEditorWidget::removeActionTriggered()
{
    if(ui->tableView->selectionModel()->selectedIndexes().count() == 1){
        int removeIndex = ui->tableView->selectionModel()->selectedIndexes().at(0).row();

        int i=0;
        int ii=0;
        CompetitionInfo * competitionToRemove = nullptr;
        for(auto & comp : calendar->getCompetitionsReference()){ //znajdujemy odpowiedni konkurs z listy (Bo nie pokazujemy treningów i serii próbnych)
            if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                if(i == removeIndex){
                    competitionToRemove = comp;
                    break;
                }
                i++;
            }
            ii++;
        }
        if(ii >= calendarModel->getDontModifyBefore()){
            if(competitionToRemove != nullptr){
                if(competitionToRemove->getTrialRound() != nullptr){
                    MyFunctions::removeFromVector(calendar->getCompetitionsReference(), competitionToRemove->getTrialRound());
                    delete competitionToRemove->getTrialRound();
                }
                for(auto & training : competitionToRemove->getTrainingsReference()){
                    MyFunctions::removeFromVector(calendar->getCompetitionsReference(), training);
                    delete training;
                }
                MyFunctions::removeFromVector(calendar->getCompetitionsReference(), competitionToRemove);
                delete competitionToRemove;
                calendarModel->removeRow(removeIndex);
                ui->tableView->clearSelection();
                ui->tableView->selectionModel()->select(calendarModel->index(removeIndex, 0), QItemSelectionModel::Select);
            }
            updateActualCompetitionByID();
        }
        if(calendarModel->rowCount() == 0)
        {
            calendar->getCompetitionsReference().clear();
        }
        calendar->fixAdvancementCompetitions();
        emit calendarModel->dataChanged(calendarModel->index(0, 0), calendarModel->index(calendarModel->rowCount() - 1, calendarModel->columnCount() - 1));
    }
    emit changed();
}

void CalendarEditorWidget::upActionTriggered()
{
    if(ui->tableView->selectionModel()->selectedIndexes().count() == 1){
        int moveIndex = ui->tableView->selectionModel()->selectedIndexes().at(0).row();
        if(moveIndex == 0) return;
        int i=0;
        int competitionToMoveIndexInCalendar = 0;
        CompetitionInfo * competitionToMove = nullptr;
        for(auto & comp : calendar->getCompetitionsReference()){ //znajdujemy odpowiedni konkurs z listy (Bo nie pokazujemy treningów i serii próbnych)
            if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                if(i == moveIndex){
                    competitionToMove = comp;
                    break;
                }
                i++;
            }
            competitionToMoveIndexInCalendar++;
        }
        if(competitionToMoveIndexInCalendar > calendarModel->getDontModifyBefore()){
            if(competitionToMove != nullptr){
                int previousCompetitionIndexInCalendar = -1;
                CompetitionInfo * previousCompetition = nullptr;
                previousCompetitionIndexInCalendar = competitionToMoveIndexInCalendar - 1;
                if(competitionToMove->getTrialRound() != nullptr)
                    previousCompetitionIndexInCalendar -= 1;
                previousCompetitionIndexInCalendar -= competitionToMove->getTrainingsReference().count();

                previousCompetition = calendar->getCompetitionsReference()[previousCompetitionIndexInCalendar];

                QVector<CompetitionInfo *> competitionToMoveCompetitions;
                if(competitionToMove->getTrialRound() != nullptr){
                    MyFunctions::removeFromVector(calendar->getCompetitionsReference(), competitionToMove->getTrialRound());
                    competitionToMoveCompetitions.push_back(competitionToMove->getTrialRound());
                }
                for(auto & training : competitionToMove->getTrainingsReference()){
                    MyFunctions::removeFromVector(calendar->getCompetitionsReference(), training);
                    competitionToMoveCompetitions.push_back(training);
                }

                QVector<CompetitionInfo *> previousCompetitionCompetitions;
                if(previousCompetition->getTrialRound() != nullptr){
                    MyFunctions::removeFromVector(calendar->getCompetitionsReference(), previousCompetition->getTrialRound());
                    previousCompetitionCompetitions.push_back(previousCompetition->getTrialRound());
                }
                for(auto & training : previousCompetition->getTrainingsReference()){
                    MyFunctions::removeFromVector(calendar->getCompetitionsReference(), training);
                    previousCompetitionCompetitions.push_back(training);
                }

                competitionToMoveIndexInCalendar = 0;
                for(auto & comp : calendar->getCompetitionsReference()){ //znajdujemy odpowiedni konkurs z listy (Bo nie pokazujemy treningów i serii próbnych)
                    if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                        if(competitionToMove == comp){
                            break;
                        }
                    }
                    competitionToMoveIndexInCalendar++;
                }

                calendar->getCompetitionsReference().swapItemsAt(competitionToMoveIndexInCalendar, competitionToMoveIndexInCalendar - 1);
                competitionToMoveIndexInCalendar -= 1;
                //qSwap(competitionToMoveIndexInCalendar, previousCompetitionIndexInCalendar);

                qDebug()<<"KALENDARZ:";
                debugCalendar();

                for(auto it = competitionToMoveCompetitions.begin(); it != competitionToMoveCompetitions.end(); it++){
                    calendar->getCompetitionsReference().insert(competitionToMoveIndexInCalendar, *it);
                }
                qDebug()<<"PO competitionToMoveCompetitions:";
                debugCalendar();
                previousCompetitionIndexInCalendar = 0;
                for(auto & comp : calendar->getCompetitionsReference()){ //znajdujemy odpowiedni konkurs z listy (Bo nie pokazujemy treningów i serii próbnych)
                    if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                        if(previousCompetition == comp){
                            break;
                        }
                    }
                    previousCompetitionIndexInCalendar++;
                }
                for(auto it = previousCompetitionCompetitions.begin(); it != previousCompetitionCompetitions.end(); it++){
                    calendar->getCompetitionsReference().insert(previousCompetitionIndexInCalendar, *it);
                }
                qDebug()<<"PO previousCompetitionIndexInCalendar:";
                debugCalendar();
                ui->tableView->clearSelection();
                ui->tableView->selectionModel()->select(calendarModel->index(moveIndex - 1, 0), QItemSelectionModel::Select);
                emit calendarModel->dataChanged(calendarModel->index(0, 0), calendarModel->index(calendarModel->rowCount() - 1, calendarModel->columnCount() - 1));
            }
            updateActualCompetitionByID();
        }
    }
    emit changed();
}

void CalendarEditorWidget::downActionTriggered()
{
    if(ui->tableView->selectionModel()->selectedIndexes().count() == 1){
        int moveIndex = ui->tableView->selectionModel()->selectedIndexes().at(0).row();
        if(moveIndex + 1 >= calendarModel->rowCount()) return;
        int i=0;
        int competitionToMoveIndexInCalendar = 0;
        CompetitionInfo * competitionToMove = nullptr;
        for(auto & comp : calendar->getCompetitionsReference()){ //znajdujemy odpowiedni konkurs z listy (Bo nie pokazujemy treningów i serii próbnych)
            if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                if(i == moveIndex){
                    competitionToMove = comp;
                    break;
                }
                i++;
            }
            competitionToMoveIndexInCalendar++;
        }
        if(competitionToMoveIndexInCalendar >= calendarModel->getDontModifyBefore()){
            if(competitionToMove != nullptr){
                int nextCompetitionIndexInCalendar = -1;
                CompetitionInfo * nextCompetition = nullptr;
                for(int i=competitionToMoveIndexInCalendar + 1; i<calendar->getCompetitionsReference().count(); i++){
                    if(calendar->getCompetitionsReference()[i]->getSerieType() == CompetitionInfo::Qualifications || calendar->getCompetitionsReference()[i]->getSerieType() == CompetitionInfo::Competition){
                        nextCompetition = calendar->getCompetitionsReference()[i];
                        nextCompetitionIndexInCalendar = i;
                        break;
                    }
                }

                nextCompetition = calendar->getCompetitionsReference()[nextCompetitionIndexInCalendar];

                QVector<CompetitionInfo *> competitionToMoveCompetitions;
                if(competitionToMove->getTrialRound() != nullptr){
                    MyFunctions::removeFromVector(calendar->getCompetitionsReference(), competitionToMove->getTrialRound());
                    competitionToMoveCompetitions.push_back(competitionToMove->getTrialRound());
                }
                for(auto & training : competitionToMove->getTrainingsReference()){
                    MyFunctions::removeFromVector(calendar->getCompetitionsReference(), training);
                    competitionToMoveCompetitions.push_back(training);
                }

                QVector<CompetitionInfo *> nextCompetitionCompetitions;
                if(nextCompetition->getTrialRound() != nullptr){
                    MyFunctions::removeFromVector(calendar->getCompetitionsReference(), nextCompetition->getTrialRound());
                    nextCompetitionCompetitions.push_back(nextCompetition->getTrialRound());
                }
                for(auto & training : nextCompetition->getTrainingsReference()){
                    MyFunctions::removeFromVector(calendar->getCompetitionsReference(), training);
                    nextCompetitionCompetitions.push_back(training);
                }

                competitionToMoveIndexInCalendar = 0;
                for(auto & comp : calendar->getCompetitionsReference()){ //znajdujemy odpowiedni konkurs z listy (Bo nie pokazujemy treningów i serii próbnych)
                    if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                        if(competitionToMove == comp){
                            break;
                        }
                    }
                    competitionToMoveIndexInCalendar++;
                }

                calendar->getCompetitionsReference().swapItemsAt(competitionToMoveIndexInCalendar, competitionToMoveIndexInCalendar + 1);
                competitionToMoveIndexInCalendar += 1;
                //qSwap(competitionToMoveIndexInCalendar, nextCompetitionIndexInCalendar);

                qDebug()<<"KALENDARZ:";
                debugCalendar();

                for(auto it = competitionToMoveCompetitions.begin(); it != competitionToMoveCompetitions.end(); it++){
                    calendar->getCompetitionsReference().insert(competitionToMoveIndexInCalendar, *it);
                }
                qDebug()<<"PO competitionToMoveCompetitions:";
                debugCalendar();
                nextCompetitionIndexInCalendar = 0;
                for(auto & comp : calendar->getCompetitionsReference()){ //znajdujemy odpowiedni konkurs z listy (Bo nie pokazujemy treningów i serii próbnych)
                    if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                        if(nextCompetition == comp){
                            break;
                        }
                    }
                    nextCompetitionIndexInCalendar++;
                }
                for(auto it = nextCompetitionCompetitions.begin(); it != nextCompetitionCompetitions.end(); it++){
                    calendar->getCompetitionsReference().insert(nextCompetitionIndexInCalendar, *it);
                }
                qDebug()<<"PO nextCompetitionIndexInCalendar:";
                debugCalendar();
                ui->tableView->clearSelection();
                ui->tableView->selectionModel()->select(calendarModel->index(moveIndex + 1, 0), QItemSelectionModel::Select);
                emit calendarModel->dataChanged(calendarModel->index(0, 0), calendarModel->index(calendarModel->rowCount() - 1, calendarModel->columnCount() - 1));
            }
            updateActualCompetitionByID();
        }
    }
    emit changed();
}

void CalendarEditorWidget::editActionTriggered()
{
    if(ui->tableView->selectionModel()->selectedIndexes().count() > 0){
        int column = ui->tableView->selectionModel()->selectedIndexes().at(0).column();
        for(auto & idx : ui->tableView->selectionModel()->selectedIndexes())
            if(idx.column() != column) return;
        QVector<int> rows;
        for(auto & idx : ui->tableView->selectionModel()->selectedIndexes()){
            if(idx.column() != column)
                return;
            rows.push_back(idx.row());
        }

        int minRow = 0;
        int i=0;
        for(auto & comp : calendar->getCompetitionsReference())
        {
            if(i == calendarModel->getDontModifyBefore())
                break;
            if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition)
                minRow++;
            i++;
        }

        for(auto & row : ui->tableView->selectionModel()->selectedIndexes())
            if(row.row() < minRow) return;

        switch(column){
        case 0: //Skocznia
        {
            execMultipleHillEditDialog(&rows, column);
            break;
        }
        case 1: //Rodzaj (Ind/Druż)
        {
            multipleEditCompetitionTypes(&rows, column);;
            break;
        }
        case 2: //Seria (Konkurs/Kwalifikacje)
        {
            multipleEditSerieTypes(&rows, column);
            break;
        }
        case 3: //Próbna (Tak/Nie)
        {
            execMultipleTrialRoundsEditDialog(&rows, column);
            break;
        }
        case 4: //Treningi
        {
            execMultipleTrainingsEditDialog(&rows, column);
            break;
        }
        case 5: //Zasady
        {
            execMultipleCompetitionRulesEditDialog(&rows, column);
            break;
        }
        case 6:
        {
            execMultipleClassificationsEditDialog(&rows, column);
            break;
        }
        case 7:
        {
            execMultipleAdvancementCompetitionEditDialog(&rows, column);
            break;
        }
        case 8:
        {
            execMultipleAdvancementClassificationEditDialog(&rows, column);
            break;
        }
        case 9:
        {
            execJumpsImportanceEditDialog(&rows, column);
            break;
        }
        }
        ui->tableView->resizeColumnsToContents();
    }
    emit changed();
}

void CalendarEditorWidget::duplicateActionTriggered()
{
    int insertIndex = 0;
    if(ui->tableView->selectionModel()->selectedIndexes().count() > 0)
        insertIndex = ui->tableView->selectionModel()->selectedIndexes().at(0).row() + 0;
    else if(calendarModel->rowCount() > 0)
        insertIndex = calendarModel->rowCount() - 1;
    else
        insertIndex = 0;

    int insertIndexInCalendar=0;
    int i=0;
    for(auto & comp : calendar->getCompetitionsReference())
    {
        if(i == insertIndex + 1){
            break;
        }
        if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition)
            i++;
        insertIndexInCalendar++;
    }
    if(insertIndexInCalendar >= calendarModel->getDontModifyBefore()){
        qDebug()<<"INSIDX "<<insertIndexInCalendar;
        insertIndexInCalendar -= 1;
        CompetitionInfo * existing = calendar->getCompetitionsReference()[insertIndexInCalendar];
        debugCalendar();
        calendarModel->insertRow(insertIndex);
        CompetitionInfo * comp = new CompetitionInfo(existing->getHill());
        comp->reassign();
        comp->setSerieType(existing->getSerieType());
        comp->setRules(existing->getRules());
        comp->setClassifications(existing->getClassifications());
        comp->setAdvancementCompetition(existing->getAdvancementCompetition());
        comp->setAdvancementClassification(existing->getAdvancementClassification());
        for(auto & tr : existing->getTrainingsReference())
        {
            CompetitionInfo * training = new CompetitionInfo();
            training->setHill(tr->getHill());
            training->setRules(tr->getRules());
            training->setSerieType(CompetitionInfo::Training);
            comp->getTrainingsReference().push_back(training);
            calendar->getCompetitionsReference().insert(insertIndexInCalendar, training);
            //insertIndexInCalendar += 1;
        }
        if(existing->getTrialRound() != nullptr){
        CompetitionInfo * trial = new CompetitionInfo();
        trial->setHill(existing->getHill());
        trial->setRules(existing->getTrialRound()->getRules());
        trial->setSerieType(CompetitionInfo::TrialRound);
        comp->setTrialRound(trial);
        calendar->getCompetitionsReference().insert(insertIndexInCalendar, trial);
        //insertIndexInCalendar += 1;
        }
        calendar->getCompetitionsReference().insert(insertIndexInCalendar, comp);
        debugCalendar();

        ui->tableView->clearSelection();
        ui->tableView->selectionModel()->select(calendarModel->index(insertIndex + 1, 0), QItemSelectionModel::Select);
        emit calendarModel->dataChanged(calendarModel->index(0, 0), calendarModel->index(calendarModel->rowCount() - 1, calendarModel->columnCount() - 1));
        ui->tableView->resizeColumnsToContents();
        updateActualCompetitionByID();
    }
    emit changed();
}

void CalendarEditorWidget::execMultipleTrainingsEditDialog(QVector<int> *rows, int column)
{
    MultipleTrainingsEditDialog dialog(this);
    dialog.setRulesList(calendarModel->getRulesList());
    CompetitionInfo * competition;
    int i=-1;
    for(auto & comp : calendar->getCompetitionsReference())
    {
        if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition)
            i++;
        if(i == rows->first())
        {
            competition = comp;
            break;
        }
    }
    if(competition->getTrainingsReference().count() > 0){
        dialog.setTrainingsRules(competition->getTrainingsReference().first()->getRules());
        dialog.setImp(competition->getTrainingsReference().first()->getJumpsImportance());
    }
    dialog.setCount(competition->getTrainingsReference().count());
    connect(&dialog, &MultipleTrainingsEditDialog::submitted, &dialog, &MultipleTrainingsEditDialog::accept);
    if(dialog.exec() == QDialog::Accepted){
        for(auto & row : qAsConst(*rows)){
            CompetitionInfo * competition = nullptr;
            int competitionIndex = 0;
            int i=0;
            for(auto & comp : calendar->getCompetitionsReference()){
                if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                    if(i == row){
                        competition = comp;
                        break;
                    }
                    i++;
                }
                competitionIndex ++;
            }
            if(competition != nullptr){
                for(auto & training : competition->getTrainingsReference()){
                    MyFunctions::removeFromVector(competition->getTrainingsReference(), training);
                    MyFunctions::removeFromVector(calendar->getCompetitionsReference(), training);
                    delete training;
                    competitionIndex--;
                }
                for(int i=0; i<dialog.getTrainingsCount(); i++){
                    CompetitionInfo * training = new CompetitionInfo(competition->getHill());
                    training->setSerieType(CompetitionInfo::Training);
                    training->setRules(dialog.getTrainingsRules());
                    training->setJumpsImportance(dialog.getJumpsImportance());
                    qDebug()<<"AAA: "<<dialog.getTrainingsRules().getName();
                    competition->getTrainingsReference().insert(0, training);
                    if(competition->getTrialRound() != nullptr)
                        calendar->getCompetitionsReference().insert(competitionIndex - 1, training);
                    else
                        calendar->getCompetitionsReference().insert(competitionIndex, training);
                }
                emit calendarModel->dataChanged(calendarModel->index(competitionIndex, 4), calendarModel->index(4, calendarModel->columnCount() - 1));
            }
        }
    }
}

void CalendarEditorWidget::execMultipleTrialRoundsEditDialog(QVector<int> *rows, int column)
{
    MultipleTrialRoundsEditDialog dialog(this);
    dialog.setRulesList(calendarModel->getRulesList());
    CompetitionInfo * competition;
    int i=-1;
    for(auto & comp : calendar->getCompetitionsReference())
    {
        if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition)
            i++;
        if(i == rows->first())
        {
            competition = comp;
            break;
        }
    }
    if(competition->getTrialRound() != nullptr){
        dialog.setTrialRoundRules(competition->getTrialRound()->getRules());
        dialog.setImp(competition->getTrialRound()->getJumpsImportance());
    }
    dialog.setTrialRound(competition->getTrialRound() != nullptr);
    connect(&dialog, &MultipleTrialRoundsEditDialog::submitted, &dialog, &MultipleTrialRoundsEditDialog::accept);
    if(dialog.exec() == QDialog::Accepted){
        for(auto & row : qAsConst(*rows)){
            CompetitionInfo * competition = nullptr;
            int competitionIndex = 0;
            int i=0;
            for(auto & comp : calendar->getCompetitionsReference()){
                if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                    if(i == row){
                        competition = comp;
                        break;
                    }
                    i++;
                }
                competitionIndex ++;
            }
            if(competition != nullptr){
                if(competition->getTrialRound() != nullptr){
                    MyFunctions::removeFromVector(calendar->getCompetitionsReference(), competition->getTrialRound());
                    delete competition->getTrialRound();
                    competition->setTrialRound(nullptr);
                    competitionIndex--;
                }
                if(dialog.getTrialRoundChecked() == true){
                    CompetitionInfo * trialRound = new CompetitionInfo(competition->getHill());
                    trialRound->setSerieType(CompetitionInfo::TrialRound);
                    trialRound->setRules(dialog.getTrialRoundRules());
                    trialRound->setJumpsImportance(dialog.getJumpsImportance());
                    competition->setTrialRound(trialRound);
                    calendar->getCompetitionsReference().insert(competitionIndex, trialRound);
                    competitionIndex++;
                }
                emit calendarModel->dataChanged(calendarModel->index(competitionIndex, 4), calendarModel->index(4, calendarModel->columnCount() - 1));
            }
        }
    }
}

void CalendarEditorWidget::multipleEditCompetitionTypes(QVector<int> *rows, int column)
{
    for(auto & row : qAsConst(*rows)){
        CompetitionInfo * competition = nullptr;
        int competitionIndex = 0;
        int i=0;
        for(auto & comp : calendar->getCompetitionsReference()){
            if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                if(i == row){
                    competition = comp;
                    break;
                }
                i++;
            }
            competitionIndex ++;
        }
        if(competition->getRulesPointer()->getCompetitionType() == CompetitionRules::Individual)
            competition->getRulesPointer()->setCompetitionType(CompetitionRules::Team);
        else
            competition->getRulesPointer()->setCompetitionType(CompetitionRules::Individual);
        calendar->fixCompetitionsClassifications();
        calendar->fixAdvancementClassifications();
        calendar->fixAdvancementCompetitions();
        emit calendarModel->dataChanged(calendarModel->index(0, 0), calendarModel->index(calendarModel->rowCount() - 1, calendarModel->columnCount() - 1));
    }
}

void CalendarEditorWidget::multipleEditSerieTypes(QVector<int> *rows, int column)
{
    for(auto & row : qAsConst(*rows)){
        CompetitionInfo * competition = nullptr;
        int competitionIndex = 0;
        int i=0;
        for(auto & comp : calendar->getCompetitionsReference()){
            if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                if(i == row){
                    competition = comp;
                    break;
                }
                i++;
            }
            competitionIndex ++;
        }
        if(competition->getSerieType() == CompetitionInfo::Competition)
            competition->setSerieType(CompetitionInfo::Qualifications);
        else
            competition->setSerieType(CompetitionInfo::Competition);
        emit calendarModel->dataChanged(calendarModel->index(competitionIndex, 4), calendarModel->index(4, calendarModel->columnCount() - 1));
    }
}

void CalendarEditorWidget::execMultipleAdvancementCompetitionEditDialog(QVector<int> *rows, int column)
{
    int row = rows->at(0);
    CompetitionInfo * competition = SeasonCalendar::getMainCompetitionByIndex(calendar->getCompetitionsReference(), row);

    QDialog * dialog = new QDialog(this);
    dialog->setWindowFlags(Qt::Window);
    dialog->setWindowTitle(tr("Konkurs"));
    dialog->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    dialog->setLayout(new QVBoxLayout(this));
    dialog->setStyleSheet("background-color: white; color: black;");

    QWidget * widget = new QWidget(dialog);
    QHBoxLayout * layout = new QHBoxLayout(dialog);
    QLabel * label = new QLabel(dialog);
    label->setText("Konkurs: ");
    QFont font("Ubuntu", 10);
    font.setBold(true);
    label->setFont(font);
    QComboBox * comboBox = new QComboBox(dialog);
    comboBox->addItem("BRAK");
    QVector<CompetitionInfo *> comps = CompetitionInfo::getSpecificTypeMainCompetitions(calendar->getCompetitionsReference(), competition->getRulesPointer()->getCompetitionType());
    int maxMainIndex = row;//SeasonCalendar::getCompetitionMainIndex(comps, SeasonCalendar::getMainCompetitionByIndex(calendar->getCompetitionsReference(), row));
    // comps to tylko konkursy indywidualne i tylko kwalifikacje lub konkurs.
    // czego szukamy w maxMainIndex? Maksymalnego wiersza w tabeli odejmując drużynówki
    // nie działa bo row to 2 a drugi element w kalendarzu to drużynówka. a jak przefiltrujemy tylko na indwydiaulne to nie ma szans że ten z tamtej funkcji tam będzie
    for(auto & comp : comps){
        if((comp->getSerieType() == CompetitionInfo::Competition || comp->getSerieType() == CompetitionInfo::Qualifications) && (competition->getRulesPointer()->getCompetitionType() == comp->getRulesPointer()->getCompetitionType()))
        {
            int mainIndex = SeasonCalendar::getCompetitionMainIndex(calendar->getCompetitionsReference(), comp);
            int displayIndex = SeasonCalendar::getCompetitionMainIndex(calendar->getCompetitionsReference(), comp) + 1;
            if(mainIndex < maxMainIndex)
            {
                comboBox->addItem(CountryFlagsManager::getFlagPixmap(comp->getHill()->getCountryCode().toLower()), QString::number(displayIndex) + ". " + comp->getHill()->getName() + " HS" + QString::number(comp->getHill()->getHSPoint()));
            }
            else break;
        }
    }
    comboBox->setCurrentIndex(comboBox->count() - 1);
    layout->addWidget(label);
    layout->addWidget(comboBox);
    widget->setLayout(layout);
    dialog->layout()->addWidget(widget);

    QHBoxLayout * l = new QHBoxLayout(dialog);
    l->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QPushButton * button = new QPushButton("OK", this);
    button->setStyleSheet("QPushButton{\nborder: 1px solid rgb(0, 59, 23);\nborder-radius: 6px;\ncolor: rgb(0, 0, 0);\nbackground-color: rgb(123, 220, 144);\n}\nQPushButton:hover{\nbackground-color: rgb(153, 253, 174);\n}");
    connect(button, &QPushButton::clicked, dialog, &QDialog::accept);
    button->setFont(QFont("Ubuntu", 12));
    l->addWidget(button);
    l->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QWidget * w = new QWidget(dialog);
    w->setLayout(l);
    dialog->layout()->addWidget(w);

    if(dialog->exec() == QDialog::Accepted){
        if(comboBox->currentIndex() > 0){
            CompetitionInfo * advancementCompetition = SeasonCalendar::getMainCompetitionByIndex(comps, comboBox->currentIndex() - 1);
            competition->setAdvancementCompetition(advancementCompetition);
        }
        else competition->setAdvancementCompetition(nullptr);
        calendar->updateCompetitionsQualifyingCompetitions();
        emit calendarModel->dataChanged(calendarModel->index(0, 0), calendarModel->index(calendarModel->rowCount() - 1, calendarModel->columnCount() - 1));
    }
}

void CalendarEditorWidget::execMultipleAdvancementClassificationEditDialog(QVector<int> *rows, int column)
{
    QDialog * dialog = new QDialog(this);
    dialog->setWindowFlags(Qt::Window);
    dialog->setWindowTitle(tr("Klasyfikacja"));
    dialog->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    dialog->setLayout(new QVBoxLayout(this));
    dialog->setStyleSheet("background-color: white; color: black;");

    QWidget * widget = new QWidget(dialog);
    QHBoxLayout * layout = new QHBoxLayout(dialog);
    QLabel * label = new QLabel(dialog);
    label->setText("Klasyfikacja: ");
    QFont font("Ubuntu", 10);
    font.setBold(true);
    label->setFont(font);
    QComboBox * comboBox = new QComboBox(dialog);
    int pos = 1;
    comboBox->addItem("BRAK");
    for(auto & cls : calendar->getClassificationsReference()){
        comboBox->addItem(QString::number(pos) + ". " + cls->getName());
        pos++;
    }
    layout->addWidget(label);
    layout->addWidget(comboBox);
    widget->setLayout(layout);
    dialog->layout()->addWidget(widget);

    QHBoxLayout * l = new QHBoxLayout(dialog);
    l->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QPushButton * button = new QPushButton("OK", this);
    button->setStyleSheet("QPushButton{\nborder: 1px solid rgb(0, 59, 23);\nborder-radius: 6px;\ncolor: rgb(0, 0, 0);\nbackground-color: rgb(123, 220, 144);\n}\nQPushButton:hover{\nbackground-color: rgb(153, 253, 174);\n}");
    connect(button, &QPushButton::clicked, dialog, &QDialog::accept);
    button->setFont(QFont("Ubuntu", 12));
    l->addWidget(button);
    l->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QWidget * w = new QWidget(dialog);
    w->setLayout(l);
    dialog->layout()->addWidget(w);

    if(dialog->exec() == QDialog::Accepted){
        Classification * advancementClassification = nullptr;
        if(comboBox->currentIndex() > 0)
            advancementClassification = calendar->getClassificationsReference()[comboBox->currentIndex() - 1];

        for(auto & row : qAsConst(*rows)){
            CompetitionInfo * competition = nullptr;
            int ii=0;
            for(auto & comp : calendar->getCompetitionsReference()){
                if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                    if(ii == row){
                        competition = comp;
                        break;
                    }
                    ii++;
                }
            }
            competition->setAdvancementClassification(advancementClassification);
        }
        calendar->fixAdvancementClassifications();
        emit calendarModel->dataChanged(calendarModel->index(0, 0), calendarModel->index(calendarModel->rowCount() - 1, calendarModel->columnCount() - 1));
    }
}

void CalendarEditorWidget::execJumpsImportanceEditDialog(QVector<int> *rows, int column)
{
    QDialog * dialog = new QDialog(this);
    dialog->setModal(true);
    dialog->setWindowFlags(Qt::Window);
    dialog->setWindowTitle(tr("Ważność skoków"));
    dialog->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    dialog->setLayout(new QVBoxLayout(this));
    dialog->setStyleSheet("background-color: white; color: black;");

    QWidget * widget = new QWidget(dialog);
    QHBoxLayout * layout = new QHBoxLayout(dialog);
    QLabel * label = new QLabel(dialog);
    label->setText("Preset: ");
    QFont font("Ubuntu", 10);
    font.setBold(true);
    label->setFont(font);
    QComboBox * comboBox = new QComboBox(dialog);
    comboBox->addItem(tr("BRAK"));
    for(auto & preset : GlobalDatabase::get()->getJumpsImportancePresetsReference()){
        comboBox->addItem(preset.getName() + " (" + QString::number(preset.getJumpsImportance(), 'f', 2) +")");
    }
    layout->addWidget(label);
    layout->addWidget(comboBox);
    widget->setLayout(layout);

    QDoubleSpinBox * spinBox = new QDoubleSpinBox(this);
    spinBox->setValue(SeasonCalendar::getMainCompetitionByIndex(calendar->getCompetitionsReference(), rows->first())->getJumpsImportance());
    spinBox->setFont(QFont("Ubuntu", 11));
    spinBox->setStyleSheet("color: black;");

    connect(comboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [spinBox](int index){
        if(index > 0)
            spinBox->setValue(GlobalDatabase::get()->getJumpsImportancePresetsReference()[index - 1].getJumpsImportance());
        else
            spinBox->setValue(5);
    });

    dialog->layout()->addWidget(widget);
    dialog->layout()->addWidget(spinBox);

    connect(dialog, &QDialog::rejected, dialog,&QDialog::accept);

    if(dialog->exec() == QDialog::Accepted)
    {
        for(auto & row : qAsConst(*rows)){
            CompetitionInfo * competition = SeasonCalendar::getMainCompetitionByIndex(calendar->getCompetitionsReference(), row);
            competition->setJumpsImportance(spinBox->value());
        }
        emit calendarModel->dataChanged(calendarModel->index(0, 0), calendarModel->index(calendarModel->rowCount() - 1, calendarModel->columnCount() - 1));
    }
}

void CalendarEditorWidget::execMultipleCompetitionRulesEditDialog(QVector<int> * rows, int column)
{
    QDialog * dialog = new QDialog(this);
    dialog->setModal(true);
    dialog->setWindowFlags(Qt::Window);
    dialog->setWindowTitle(tr("Zasady konkursów"));
        dialog->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    dialog->setLayout(new QVBoxLayout(this));
    dialog->setStyleSheet("background-color: white; color: black;");

    CompetitionRulesEditorWidget * editor = new CompetitionRulesEditorWidget(this);
    editor->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    CompetitionInfo * competition;
    int i=-1;
    for(auto & comp : calendar->getCompetitionsReference())
    {
        if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition)
            i++;
        if(i == rows->first())
        {
            competition = comp;
            break;
        }
    }
    editor->setCompetitionRules(competition->getRulesPointer());
    editor->fillCompetitionRulesInputs();

    QWidget * widget = new QWidget(dialog);
    QHBoxLayout * layout = new QHBoxLayout(dialog);
    QLabel * label = new QLabel(dialog);
    label->setText("Zasady: ");
    QFont font("Ubuntu", 10);
    font.setBold(true);
    label->setFont(font);
    QComboBox * comboBox = new QComboBox(dialog);
    comboBox->addItem(tr("BRAK"));
    for(auto & rules : *calendarModel->getRulesList()){
        comboBox->addItem(rules.getName());
    }
    layout->addWidget(label);
    layout->addWidget(comboBox);
    widget->setLayout(layout);
    connect(comboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, editor, comboBox](){
        if(comboBox->currentIndex() > 0){
            CompetitionRules * rules = const_cast<CompetitionRules *>(&calendarModel->getRulesList()->at(comboBox->currentIndex() - 1));
            editor->setCompetitionRules(rules);
            editor->fillCompetitionRulesInputs();
        }
        else
            editor->resetInputs();
    });
    dialog->layout()->addWidget(widget);
    dialog->layout()->addWidget(editor);


    connect(editor, &CompetitionRulesEditorWidget::submitted, dialog, &QDialog::accept);
    if(dialog->exec() == QDialog::Accepted){
        //Dla każdego zaznaczonego konkursu ustaw zasady konkursu z edytora
        for(auto & row : qAsConst(*rows)){
            CompetitionInfo * competition = nullptr;
            int i=0;
            for(auto & comp : calendar->getCompetitionsReference()){
                if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                    if(i == row){
                        competition = comp;
                        break;
                    }
                    i++;
                }
            }
            if(competition != nullptr){
                competition->setRules(editor->getCompetitionRulesFromWidgetInputs());
                emit calendarModel->dataChanged(calendarModel->index(row, column), calendarModel->index(row, column));
            }
        }
        calendar->fixAdvancementClassifications();
        emit calendarModel->dataChanged(calendarModel->index(0, 8), calendarModel->index(calendarModel->rowCount() - 1, 8));
    }
}

void CalendarEditorWidget::execMultipleClassificationsEditDialog(QVector<int> *rows, int column)
{
    QDialog * dialog = new QDialog(this);
    dialog->setModal(true);
    dialog->setWindowFlags(Qt::Window);
    dialog->setWindowTitle(tr("Klasyfikacje"));
    dialog->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    dialog->setLayout(new QVBoxLayout(this));
    dialog->setStyleSheet("background-color: white; color: black;");

    CompetitionInfo * competition = SeasonCalendar::getMainCompetitionByIndex(calendar->getCompetitionsReference(), rows->first());

    QVBoxLayout * checkBoxesLayout = new QVBoxLayout(dialog);
    for(int i=0; i<save->getActualSeason()->getActualCalendar()->getClassificationsReference().count(); i++){
        Classification * c = const_cast<Classification *>(save->getActualSeason()->getActualCalendar()->getClassificationsReference()[i]);
        QCheckBox * checkBox = new QCheckBox(dialog);
        checkBox->setChecked(competition->getClassificationsReference().contains(c));
        QString text = c->getName();
        if(c->getClassificationType() == Classification::Individual)
            text += tr(" (Indywidualne)");
        else if(c->getClassificationType() == Classification::Team)
            text += tr(" (Drużynowe)");
                    checkBox->setText(text);
        checkBoxesLayout->addWidget(checkBox);
    }

    QWidget * widget = new QWidget(dialog);
    widget->setLayout(checkBoxesLayout);

    QPushButton * button = new QPushButton("OK", this);
    button->setStyleSheet("QPushButton{\nborder: 1px solid rgb(0, 59, 23);\nborder-radius: 6px;\ncolor: rgb(0, 0, 0);\nbackground-color: rgb(123, 220, 144);\n}\nQPushButton:hover{\nbackground-color: rgb(153, 253, 174);\n}");
    button->setFont(QFont("Ubuntu", 12));
    connect(button, &QPushButton::clicked, dialog, &QDialog::accept);

    dialog->layout()->addWidget(widget);
    dialog->layout()->addWidget(button);

    //iterujemy przez wszystkie comboBoxy. Jeżeli zaznaczony to iterujemy przez wszystkie zaznaczone konkursy
    //Musimy wyczyścić listę klasyfikacji dla konkursu przed rozpoczęciem dodawania

    if(dialog->exec() == QDialog::Accepted){
        for(int i=0; i<checkBoxesLayout->count(); i++){
            for(auto & row : qAsConst(*rows)){
                CompetitionInfo * competition = nullptr;
                int competitionIndex = 0;
                int ii=0;
                for(auto & comp : calendar->getCompetitionsReference()){
                    if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                        if(ii == row){
                            competition = comp;
                            break;
                        }
                        ii++;
                    }
                    competitionIndex ++;
                }
                if(i==0)
                    competition->getClassificationsReference().clear();

                if(static_cast<QCheckBox *>(checkBoxesLayout->itemAt(i)->widget())->isChecked() == true){
                    competition->getClassificationsReference().push_back(const_cast<Classification *>(calendar->getClassificationsReference()[i]));
                    emit calendarModel->dataChanged(calendarModel->index(competitionIndex, 0), calendarModel->index(0, calendarModel->columnCount() - 1));
                }
            }
        }
        calendar->fixCompetitionsClassifications();
        emit calendarModel->dataChanged(calendarModel->index(0, 6), calendarModel->index(calendarModel->rowCount() - 1, 6));
    }
}

void CalendarEditorWidget::execMultipleHillEditDialog(QVector<int> *rows, int column)
{
    QDialog * dialog = new QDialog(this);
    dialog->setWindowFlags(Qt::Window);
    dialog->setWindowTitle(tr("Skocznia"));
    dialog->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    dialog->setLayout(new QVBoxLayout(this));
    dialog->setStyleSheet("background-color: white; color: black;");

    QWidget * widget = new QWidget(dialog);
    QHBoxLayout * layout = new QHBoxLayout(dialog);
    QLabel * label = new QLabel(dialog);
    label->setText("Skocznia: ");
    QFont font("Ubuntu", 10);
    font.setBold(true);
    label->setFont(font);
    QComboBox * comboBox = new QComboBox(dialog);
    for(auto & hill : *calendarModel->getHillsList()){
        comboBox->addItem(CountryFlagsManager::getFlagPixmap(hill->getCountryCode().toLower()), hill->getName() + " HS" + QString::number(hill->getHSPoint()));
    }
    layout->addWidget(label);
    layout->addWidget(comboBox);
    widget->setLayout(layout);
    dialog->layout()->addWidget(widget);

    QHBoxLayout * l = new QHBoxLayout(dialog);
    l->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QPushButton * button = new QPushButton("OK", this);
    button->setStyleSheet("QPushButton{\nborder: 1px solid rgb(0, 59, 23);\nborder-radius: 6px;\ncolor: rgb(0, 0, 0);\nbackground-color: rgb(123, 220, 144);\n}\nQPushButton:hover{\nbackground-color: rgb(153, 253, 174);\n}");
    connect(button, &QPushButton::clicked, dialog, &QDialog::accept);
    button->setFont(QFont("Ubuntu", 12));
    l->addWidget(button);
    l->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QWidget * w = new QWidget(dialog);
    w->setLayout(l);
    dialog->layout()->addWidget(w);

    if(dialog->exec() == QDialog::Accepted){
        Hill * hill = const_cast<Hill*>(calendarModel->getHillsList()->at(comboBox->currentIndex()));
        for(auto & row : qAsConst(*rows)){
            CompetitionInfo * competition = nullptr;
            int competitionIndex = 0;
            int i=0;
            for(auto & comp : calendar->getCompetitionsReference()){
                if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
                    if(i == row){
                        competition = comp;
                        break;
                    }
                    i++;
                }
                competitionIndex ++;
            }
            competition->setHill(hill);
            for(auto & t : competition->getTrainingsReference())
                t->setHill(hill);
            if(competition->getTrialRound() != nullptr)
                competition->getTrialRound()->setHill(hill);
        }
        emit calendarModel->dataChanged(calendarModel->index(0, 0), calendarModel->index(calendarModel->rowCount() - 1, calendarModel->columnCount() - 1));
    }
}

/*void CalendarEditorWidget::on_tableView_doubleClicked(const QModelIndex &index)
{
    CompetitionInfo * competition = nullptr;
    int i=0;
    for(auto & comp : calendar->getCompetitionsReference()){
        if(comp->getSerieType() == CompetitionInfo::Qualifications || comp->getSerieType() == CompetitionInfo::Competition){
            if(i == index.row()){
                competition = comp;
                actualCompetitionID = competition->getID();
                qDebug()<<"actualCompetitionID: "<<actualCompetitionID;
                updateActualCompetitionByID();
                break;
            }
            i++;
        }
    }
    if(competition != nullptr){
        competitionInfoEditor->setCompetitionInfo(competition);
        if(competition->getTrainingsReference().count() > 0)
            competitionInfoEditor->setTrainingsRules(competition->getTrainingsReference()[0]->getRules());
        if(competition->getTrialRound() != nullptr)
            competitionInfoEditor->setTrialRoundRules(competition->getTrialRound()->getRules());
        competitionInfoEditor->fillInputs();
        // if(competitionInfoEditor->isHidden())
        //   competitionInfoEditor->show();
    }
    else{
        actualCompetitionID = (-1);
        updateActualCompetitionByID();
    }
}*/

void CalendarEditorWidget::on_pushButton_saveCalendarPreset_clicked()
{
    bool ok = false;
    QString text = QInputDialog::getText(this, tr("Nazwa presetu"), tr("Podaj nazwę jaką ma mieć ten preset kalendarza. Jeżeli istnieje już preset o takiej nazwie, zostanie on nadpisany."), QLineEdit::Normal, "", &ok);
    if(ok == true)
    {
        bool textExsists = false;
        int index=0;
        for(auto & p : GlobalDatabase::get()->getEditableGlobalCalendarPresets())
        {
            if(p.getName() == text){
                textExsists = true;
                break;
            }
            index++;
        }
        if(textExsists == false)
            index = (-1);

        SeasonCalendarPreset * preset;
        if(index == -1){
            GlobalDatabase::get()->getEditableGlobalCalendarPresets().push_back(SeasonCalendarPreset(this->calendar));
            preset = &GlobalDatabase::get()->getEditableGlobalCalendarPresets().last();
        }
        else{
            GlobalDatabase::get()->getEditableGlobalCalendarPresets()[index] = SeasonCalendarPreset(this->calendar);
            preset = &GlobalDatabase::get()->getEditableGlobalCalendarPresets()[index];
        }
        preset->setName(text);
        SeasonCalendar * calendar = &preset->getCalendarReference();
        for(auto & comp : calendar->getCompetitionsReference())
        {
            comp->reassign();
            comp->getResultsReference().reassign();
            comp->setPlayed(false);
            comp->setCancelled(false);
            comp->getResultsReference().getResultsReference().clear();
            comp->getRoundsKOGroupsReference().clear();
            comp->getTeamsReference().clear();
            comp->getTrainingsReference().detach();
            QPair<QString, double> hillPair = {comp->getHill()->getName(), comp->getHill()->getHSPoint()};
            preset->getHillsReference().push_back(hillPair);
            comp->setHill(nullptr);
        }
        for(auto & cls : calendar->getClassificationsReference())
        {
            cls->reassign();
            cls->getResultsReference().clear();
        }
        GlobalDatabase::get()->writeCalendarPresets();
    }
}

void CalendarEditorWidget::on_pushButton_loadFromPreset_clicked()
{
    if(save->getNextCompetitionIndex() > 0)
    {
        QMessageBox::information(this, tr("Preset kalendarza"), tr("Niestety nie można wczytać presetu kalendarza, ponieważ zostały rozegrane już konkursy. Stwórz inny kalendarz i spróbuj ponownie."), QMessageBox::Ok);
return;
    }
    else
    {
        bool ok = false;
        QStringList presetsNames;
        for(auto & preset : GlobalDatabase::get()->getEditableGlobalCalendarPresets())
            presetsNames.push_back(preset.getName());
        QString itemText = QInputDialog::getItem(this, tr("Wybierz preset kalendarza"), tr("Wybierz z listy preset kalendarza który chcesz wczytać"), presetsNames,0, false, &ok);

    if(ok == true)
    {
        SeasonCalendarPreset * preset = &GlobalDatabase::get()->getEditableGlobalCalendarPresets()[presetsNames.indexOf(itemText)];
        for(auto & comp : calendar->getCompetitionsReference())
        {
            MyFunctions::removeFromVector(calendar->getCompetitionsReference(), comp);
            delete comp;
        }
        QHash<CompetitionInfo *, CompetitionInfo *> competitions; //1. Konkurs z presetu, 2. Nowy konkurs z kalendarza
        for(auto & presetCompetition : preset->getCalendarReference().getCompetitionsReference())
        {
            CompetitionInfo * calendarCompetition = new CompetitionInfo(*presetCompetition);
            calendarCompetition->reassign();
            calendarCompetition->getResultsReference().reassign();

            calendarCompetition->getClassificationsReference().detach();
            calendarCompetition->getQualifyingCompetitionsReference().detach();
            calendarCompetition->getResultsReference().getResultsReference().detach();
            calendarCompetition->getRoundsKOGroupsReference().detach();
            calendarCompetition->getTeamsReference().detach();
            calendarCompetition->getTrainingsReference().detach();
            calendar->getCompetitionsReference().push_back(calendarCompetition);
            competitions.insert(presetCompetition, calendarCompetition);
        }
        for(auto & calendarCompetition : calendar->getCompetitionsReference())
        {
            for(auto & presetCompetition : preset->getCalendarReference().getCompetitionsReference())
            {
                    if(calendarCompetition->getAdvancementCompetition() == presetCompetition)
                        calendarCompetition->setAdvancementCompetition(competitions.value(presetCompetition));
                    if(calendarCompetition->getTrialRound() == presetCompetition)
                        calendarCompetition->setTrialRound(competitions.value(presetCompetition));
                    for(auto & calendarTraining : calendarCompetition->getTrainingsReference())
                    {
                        if(calendarTraining == presetCompetition){
                            calendarTraining = competitions.value(presetCompetition);
                        }
                    }
            }
        }
        for(auto & cls : calendar->getClassificationsReference())
        {
            MyFunctions::removeFromVector(calendar->getClassificationsReference(), cls);
            delete cls;
        }
        QHash<Classification *, Classification *> classifications; //1. Klasyfikacja z presetu, 2. Nowa Klasyfikacja z kalendarza
        for(auto & presetClassification : preset->getCalendarReference().getClassificationsReference())
        {
            Classification * calendarClassification = new Classification(*presetClassification);
            calendarClassification->reassign();

            calendarClassification->getPointsForPlacesReference().detach();
            calendarClassification->getResultsReference().detach();
            calendar->getClassificationsReference().push_back(calendarClassification);
            classifications.insert(presetClassification, calendarClassification);
        }
        for(auto & calendarCompetition : calendar->getCompetitionsReference())
        {
            for(auto & presetClassification : preset->getCalendarReference().getClassificationsReference())
            {
                    if(calendarCompetition->getAdvancementClassification() == presetClassification)
                        calendarCompetition->setAdvancementClassification(classifications.value(presetClassification));
                    for(auto & cls : calendarCompetition->getClassificationsReference())
                    {
                        if(cls == presetClassification)
                            cls = classifications.value(presetClassification);
                    }
            }
        }
        calendar->updateCompetitionsQualifyingCompetitions();
        int i=0;
        for(auto & presetHill : preset->getHillsReference())
        {
            Hill * hillToSet = nullptr;
            for(auto & hill : save->getHillsReference())
            {
                    if(hill->getName() == presetHill.first && hill->getHSPoint() == presetHill.second){
                        hillToSet = hill;
                    }
            }
            if(hillToSet != nullptr)
            {
                    calendar->getCompetitionsReference()[i]->setHill(hillToSet);
            }
            else
            {
                    calendar->getCompetitionsReference()[i]->setHill(new Hill("Hill"));
            }
            i++;
        }
        calendarModel->setCalendar(calendar);
        ui->tableView->setModel(nullptr);
        ui->tableView->setModel(calendarModel);
        ui->tableView->resizeColumnsToContents();

        emit calendarPresetLoaded();
    }
    }
}
