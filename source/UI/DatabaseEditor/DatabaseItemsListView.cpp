#include "DatabaseItemsListView.h"
#include "ui_DatabaseItemsListView.h"
#include <QMessageBox>

namespace {
template<typename T>
void swapVectorItems(QVector<T> *items, int first, int second)
{
    qSwap((*items)[first], (*items)[second]);
}
}

DatabaseItemsListView::DatabaseItemsListView(int type, bool allowInserting, bool allowRemoving, bool allowMoving, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatabaseItemsListView),
    type(type),
    allowInserting(allowInserting),
    allowRemoving(allowRemoving),
    allowMoving(allowMoving),
    insertLast(false),
    showItemsNumbers(false)
{
    ui->setupUi(this);
    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    seasonJumpers = nullptr;
    seasonHills = nullptr;
    jumpers = nullptr;
    hills = nullptr;
    competitionRules = nullptr;
    classifications = nullptr;
    pointsForPlacesPresets = nullptr;
    jumpersLists = nullptr;
    listModel = nullptr;
    calendars = nullptr;

    insertAction = new QAction(this);
    insertAction->setShortcut(Qt::CTRL | Qt::Key_A);
    this->addAction(insertAction);
    connect(insertAction, &QAction::triggered, this, &DatabaseItemsListView::onInsertActionTriggered);

    removeAction = new QAction(this);
    removeAction->setShortcut(Qt::CTRL | Qt::Key_D);
    this->addAction(removeAction);
    connect(removeAction, &QAction::triggered, this, &DatabaseItemsListView::onRemoveActionTriggered);

    upAction = new QAction(this);
    upAction->setShortcut(Qt::CTRL | Qt::Key_Up);
    this->addAction(upAction);
    connect(upAction, &QAction::triggered, this, &DatabaseItemsListView::onUpActionTriggered);

    downAction = new QAction(this);
    downAction->setShortcut(Qt::CTRL | Qt::Key_Down);
    this->addAction(downAction);
    connect(downAction, &QAction::triggered, this, &DatabaseItemsListView::onDownActionTriggered);

    connect(ui->listView, &QListView::doubleClicked, this, &DatabaseItemsListView::listViewDoubleClicked);
    connect(ui->listView, &QListView::clicked, this, &DatabaseItemsListView::listViewClicked);\

        /*connect(this, &DatabaseItemsListView::insert, this, [this](){
        if(listModel->rowCount() == 1 || listModel->rowCount() == 2)
            selectOnlyFirstRow();
    });*/
        connect(this, &DatabaseItemsListView::listViewDoubleClicked, this, [this](const QModelIndex &index){
            lastDoubleClickedIndex = index.row();
        });
}

DatabaseItemsListView::~DatabaseItemsListView()
{
    delete ui;
}

void DatabaseItemsListView::setupListModel()
{
    if(listModel != nullptr)
        delete listModel;
    switch(type){
    case SeasonJumpersItems:
        listModel = new SeasonJumpersListModel(this->seasonJumpers);
        static_cast<SeasonJumpersListModel *>(listModel)->setShowItemsNumbers(this->showItemsNumbers);
        break;
    case SeasonHillsItems:
        listModel = new SeasonHillsListModel(this->seasonHills);
        break;
    case JumperItems:
        listModel = new JumpersListModel(this->jumpers);
        break;
    case HillItems:
        listModel = new HillsListModel(this->hills);
        break;
    case CompetitionRulesItems:
        listModel = new CompetitionRulesListModel(this->competitionRules);
        break;
    case ClassificationItems:
        listModel = new ClassificationsListModel(this->classifications);
        break;
    case PointsForPlacesPresetsItems:
        listModel = new PointsForPlacesPresetsListModel(this->pointsForPlacesPresets);
        break;
    case JumpersListsItems:
        listModel = new JumpersListsListModel(this->jumpersLists);
        break;
    case CalendarItems:
        listModel = new CalendarsListModel(this->calendars);
    }
    listModel->setParent(this);
    ui->listView->setModel(listModel);
}

void DatabaseItemsListView::selectOnlyFirstRow()
{
    if(listModel->rowCount() > 0)
    {
        const QModelIndex firstIndex = listModel->index(0);
        ui->listView->setCurrentIndex(firstIndex);
        ui->listView->selectionModel()->select(firstIndex, QItemSelectionModel::ClearAndSelect);
    }
}

void DatabaseItemsListView::hideInstructions()
{
    ui->textEdit_shortcuts->hide();
}

void DatabaseItemsListView::showInstructions()
{
    ui->textEdit_shortcuts->show();
}

void DatabaseItemsListView::setRemoveKeySequence(QKeySequence sequence)
{
    removeAction->setShortcut(sequence);
}

void DatabaseItemsListView::setAddKeySequence(QKeySequence sequence)
{
    insertAction->setShortcut(sequence);
}

void DatabaseItemsListView::setUpKeySequence(QKeySequence sequence)
{
    upAction->setShortcut(sequence);
}

void DatabaseItemsListView::setDownKeySequence(QKeySequence sequence)
{
    downAction->setShortcut(sequence);
}

QVector<SeasonCalendar *> *DatabaseItemsListView::getCalendars() const
{
    return calendars;
}

void DatabaseItemsListView::setCalendars(QVector<SeasonCalendar *> *newCalendars)
{
    calendars = newCalendars;
}

QVector<SaveJumpersList> *DatabaseItemsListView::getJumpersLists() const
{
    return jumpersLists;
}

void DatabaseItemsListView::setJumpersLists(QVector<SaveJumpersList> *newJumpersLists)
{
    jumpersLists = newJumpersLists;
}

bool DatabaseItemsListView::getShowItemsNumbers() const
{
    return showItemsNumbers;
}

void DatabaseItemsListView::setShowItemsNumbers(bool newShowItemsNumbers)
{
    showItemsNumbers = newShowItemsNumbers;
}

QVector<Hill *> *DatabaseItemsListView::getSeasonHills() const
{
    return seasonHills;
}

void DatabaseItemsListView::setSeasonHills(QVector<Hill *> *newSeasonHills)
{
    seasonHills = newSeasonHills;
}

QVector<Classification *> *DatabaseItemsListView::getClassifications() const
{
    return classifications;
}

void DatabaseItemsListView::setClassifications(QVector<Classification *> *newClassifications)
{
    classifications = newClassifications;
}

bool DatabaseItemsListView::getInsertLast() const
{
    return insertLast;
}

void DatabaseItemsListView::setInsertLast(bool newInsertLast)
{
    insertLast = newInsertLast;
}

bool DatabaseItemsListView::getAllowMoving() const
{
    return allowMoving;
}

void DatabaseItemsListView::setAllowMoving(bool newAllowMoving)
{
    allowMoving = newAllowMoving;
}

int DatabaseItemsListView::getType() const
{
    return type;
}

void DatabaseItemsListView::setType(int newType)
{
    type = newType;
}

QVector<Jumper *> *DatabaseItemsListView::getSeasonJumpers() const
{
    return seasonJumpers;
}

void DatabaseItemsListView::setSeasonJumpers(QVector<Jumper *> *newSeasonJumpers)
{
    seasonJumpers = newSeasonJumpers;
}

int DatabaseItemsListView::getLastDoubleClickedIndex() const
{
    return lastDoubleClickedIndex;
}

void DatabaseItemsListView::setLastDoubleClickedIndex(int newLastDoubleClickedIndex)
{
    lastDoubleClickedIndex = newLastDoubleClickedIndex;
}

QVector<PointsForPlacesPreset> *DatabaseItemsListView::getPointsForPlacesPresets() const
{
    return pointsForPlacesPresets;
}

void DatabaseItemsListView::setPointsForPlacesPresets(QVector<PointsForPlacesPreset> *newPointsForPlacesPresets)
{
    pointsForPlacesPresets = newPointsForPlacesPresets;
}

QAbstractListModel *DatabaseItemsListView::getListModel()
{
    return listModel;
}

void DatabaseItemsListView::onInsertActionTriggered()
{
     QModelIndexList rows = ui->listView->selectionModel()->selectedRows();
    if(allowInserting == true){
        int count = 0;
        switch(type){
        case SeasonJumpersItems:{
            count = seasonJumpers->count();
            break;
        }
        case SeasonHillsItems:{
            count = seasonHills->count();
            break;
        }
        case JumperItems:{
            count = jumpers->count();
            break;
        }
        case HillItems:{
            count = hills->count();
            break;
        }
        case CompetitionRulesItems:{
            count = competitionRules->count();
            break;
        }
        case ClassificationItems:{
            count = classifications->count();
            break;
        }
        case PointsForPlacesPresetsItems:{
            count = pointsForPlacesPresets->count();
            break;
        }
        case JumpersListsItems:{
            count = jumpersLists->count();
            break;
        case CalendarItems:{
            count = calendars->count();
            break;
        }
        }
        default: break;
        }
        int rowToInsert = 0;
        if(count == 0)
            rowToInsert = 0;
        else if(rows.size() == 0 || insertLast == true)
            rowToInsert = count;
        else if(rows.size() == 1)
            rowToInsert = rows[0].row();

        if(allowInserting == true){
            switch(type){
            case SeasonJumpersItems:{
                SeasonJumpersListModel * jumpersListModel = dynamic_cast<SeasonJumpersListModel *>(listModel);
                jumpersListModel->insertRows(rowToInsert, 1);
                jumpersListModel->getSeasonJumpers()->insert(rowToInsert, 1, new Jumper("Name", "Surname", "XXX"));
                emit jumpersListModel->dataChanged(jumpersListModel->index(rowToInsert), jumpersListModel->index(jumpersListModel->rowCount() - 1));
                break;
            }
            case SeasonHillsItems:{
                SeasonHillsListModel * hillsListModel = dynamic_cast<SeasonHillsListModel *>(listModel);
                hillsListModel->insertRows(rowToInsert, 1);
                hillsListModel->getSeasonHills()->insert(rowToInsert, 1, new Hill("Hill"));
                emit hillsListModel->dataChanged(hillsListModel->index(rowToInsert), hillsListModel->index(hillsListModel->rowCount() - 1));
                break;
            }
            case JumperItems:{
                JumpersListModel * jumpersListModel = dynamic_cast<JumpersListModel *>(listModel);
                jumpersListModel->insertRows(rowToInsert, 1);
                jumpersListModel->getJumpersVectorPointer()->insert(rowToInsert, 1, Jumper("Name", "Surname", "XXX"));
                emit jumpersListModel->dataChanged(jumpersListModel->index(rowToInsert), jumpersListModel->index(jumpersListModel->rowCount() - 1));
                break;
            }
            case HillItems:{
                HillsListModel * hillsListModel = dynamic_cast<HillsListModel *>(listModel);
                hillsListModel->insertRows(rowToInsert, 1);
                hillsListModel->getHillsVectorPointer()->insert(rowToInsert, 1, Hill("Hill", "XXX"));
                emit hillsListModel->dataChanged(hillsListModel->index(rowToInsert), hillsListModel->index(hillsListModel->rowCount() - 1));
                break;
            }
            case CompetitionRulesItems:{
                CompetitionRulesListModel * competitionRulesListModel = dynamic_cast<CompetitionRulesListModel *>(listModel);
                competitionRulesListModel->insertRows(rowToInsert, 1);
                competitionRulesListModel->getCompetitonRulesVectorPointer()->insert(rowToInsert, 1, CompetitionRules("Rules"));
                emit competitionRulesListModel->dataChanged(competitionRulesListModel->index(rowToInsert), competitionRulesListModel->index(competitionRulesListModel->rowCount() - 1));
                break;
            }
            case ClassificationItems:{
                ClassificationsListModel * classificationsListModel = dynamic_cast<ClassificationsListModel *>(listModel);
                classificationsListModel->insertRows(rowToInsert, 1);
                classificationsListModel->getClassificationsVectorPointer()->insert(rowToInsert, 1, new Classification("Classification"));
                emit classificationsListModel->dataChanged(classificationsListModel->index(rowToInsert), classificationsListModel->index(classificationsListModel->rowCount() - 1));
                break;
            }
            case PointsForPlacesPresetsItems:{
                PointsForPlacesPresetsListModel * model = dynamic_cast<PointsForPlacesPresetsListModel *>(listModel);
                model->insertRows(rowToInsert, 1);
                model->getPresetsVectorPointer()->insert(rowToInsert, 1, PointsForPlacesPreset("Preset"));
                emit model->dataChanged(model->index(rowToInsert), model->index(model->rowCount() - 1));
                break;
            }
            case JumpersListsItems:{
                JumpersListsListModel * model = dynamic_cast<JumpersListsListModel *>(listModel);
                model->insertRows(rowToInsert, 1);
                model->getJumpersLists()->insert(rowToInsert, 1, SaveJumpersList("Jumpers List"));
                emit model->dataChanged(model->index(rowToInsert), model->index(model->rowCount() - 1));
                break;
            }
            case CalendarItems:{
                CalendarsListModel * model = dynamic_cast<CalendarsListModel *>(listModel);
                model->insertRows(rowToInsert, 1);
                model->getCalendars()->insert(rowToInsert, 1, new SeasonCalendar("Calendar"));
                emit model->dataChanged(model->index(rowToInsert), model->index(model->rowCount() - 1));
                break;
            }
            default: break;
            }
        }
    }
    QVector<int> rs;
    for(auto & r : rows)
        rs.push_back(r.row());
    emit insert(rs);
}

void DatabaseItemsListView::onRemoveActionTriggered()
{
    QModelIndexList rows = ui->listView->selectionModel()->selectedRows();
    if(allowRemoving == true){
        if(rows.size() > 0){
            int firstRow = rows.first().row();
            switch(type){
            case SeasonJumpersItems:{
                SeasonJumpersListModel * jumpersListModel = dynamic_cast<SeasonJumpersListModel *>(listModel);
                while(ui->listView->selectionModel()->selectedRows().size() > 0){
                    int rowToRemove = ui->listView->selectionModel()->selectedRows().first().row();
                    jumpersListModel->removeRows(rowToRemove, 1);
                    jumpersListModel->getSeasonJumpers()->remove(rowToRemove, 1);
                }
                emit jumpersListModel->dataChanged(jumpersListModel->index(firstRow), jumpersListModel->index(jumpersListModel->rowCount() - 1));
                break;
            }
            case SeasonHillsItems:{
                SeasonHillsListModel * hillsListModel = dynamic_cast<SeasonHillsListModel *>(listModel);
                while(ui->listView->selectionModel()->selectedRows().size() > 0){
                    int rowToRemove = ui->listView->selectionModel()->selectedRows().first().row();
                    hillsListModel->removeRows(rowToRemove, 1);
                    hillsListModel->getSeasonHills()->remove(rowToRemove, 1);
                }
                emit hillsListModel->dataChanged(hillsListModel->index(firstRow), hillsListModel->index(hillsListModel->rowCount() - 1));
                break;
            }
            case JumperItems:{
                JumpersListModel * jumpersListModel = dynamic_cast<JumpersListModel *>(listModel);
                while(ui->listView->selectionModel()->selectedRows().size() > 0){
                    int rowToRemove = ui->listView->selectionModel()->selectedRows().first().row();
                    jumpersListModel->removeRows(rowToRemove, 1);
                    jumpersListModel->getJumpersVectorPointer()->remove(rowToRemove, 1);
                }
                emit jumpersListModel->dataChanged(jumpersListModel->index(firstRow), jumpersListModel->index(jumpersListModel->rowCount() - 1));
                break;
            }
            case HillItems:{
                HillsListModel * hillsListModel = dynamic_cast<HillsListModel *>(listModel);
                while(ui->listView->selectionModel()->selectedRows().size() > 0){
                    int rowToRemove = ui->listView->selectionModel()->selectedRows().first().row();
                    hillsListModel->removeRows(rowToRemove, 1);
                    hillsListModel->getHillsVectorPointer()->remove(rowToRemove, 1);
                }
                emit hillsListModel->dataChanged(hillsListModel->index(firstRow), hillsListModel->index(hillsListModel->rowCount() - 1));
                break;
            }
            case CompetitionRulesItems:{
                CompetitionRulesListModel * competitionRulesListModel = dynamic_cast<CompetitionRulesListModel *>(listModel);
                while(ui->listView->selectionModel()->selectedRows().size() > 0){
                    int rowToRemove = ui->listView->selectionModel()->selectedRows().first().row();
                    competitionRulesListModel->removeRows(rowToRemove, 1);
                    competitionRulesListModel->getCompetitonRulesVectorPointer()->remove(rowToRemove, 1);
                }
                emit competitionRulesListModel->dataChanged(competitionRulesListModel->index(firstRow), competitionRulesListModel->index(competitionRulesListModel->rowCount() - 1));
                break;
            }
            case ClassificationItems:{
                ClassificationsListModel * classificationsListModel = dynamic_cast<ClassificationsListModel *>(listModel);
                while(ui->listView->selectionModel()->selectedRows().size() > 0){
                    int rowToRemove = ui->listView->selectionModel()->selectedRows().first().row();
                    classificationsListModel->removeRows(rowToRemove, 1);
                    classificationsListModel->getClassificationsVectorPointer()->remove(rowToRemove, 1);
                }
                emit classificationsListModel->dataChanged(classificationsListModel->index(firstRow), classificationsListModel->index(classificationsListModel->rowCount() - 1));
                break;
            }
            case PointsForPlacesPresetsItems:
            {
                PointsForPlacesPresetsListModel * model = dynamic_cast<PointsForPlacesPresetsListModel *>(listModel);
                while(ui->listView->selectionModel()->selectedRows().size() > 0){
                    int rowToRemove = ui->listView->selectionModel()->selectedRows().first().row();
                    model->removeRows(rowToRemove, 1);
                    model->getPresetsVectorPointer()->remove(rowToRemove, 1);
                }
                emit model->dataChanged(model->index(firstRow), model->index(model->rowCount() - 1));
                break;
            }
            case JumpersListsItems:
            {
                JumpersListsListModel * model = dynamic_cast<JumpersListsListModel *>(listModel);
                while(ui->listView->selectionModel()->selectedRows().size() > 0){
                    int rowToRemove = ui->listView->selectionModel()->selectedRows().first().row();
                    model->removeRows(rowToRemove, 1);
                    model->getJumpersLists()->remove(rowToRemove, 1);
                }
                emit model->dataChanged(model->index(firstRow), model->index(model->rowCount() - 1));
                break;
            }
            case CalendarItems:
            {
                CalendarsListModel * model = dynamic_cast<CalendarsListModel *>(listModel);
                while(ui->listView->selectionModel()->selectedRows().size() > 0){
                    int rowToRemove = ui->listView->selectionModel()->selectedRows().first().row();
                    model->removeRows(rowToRemove, 1);
                    model->getCalendars()->remove(rowToRemove, 1);
                }
                emit model->dataChanged(model->index(firstRow), model->index(model->rowCount() - 1));
                break;
            }
            default: break;
            }
        }
    }
    QVector<int> rs;
    for(auto & r : rows)
        rs.push_back(r.row());
    emit remove(rs);
}

void DatabaseItemsListView::onUpActionTriggered()
{
    if(allowMoving){
        QModelIndexList rows = ui->listView->selectionModel()->selectedRows();
        if(rows.size() > 0){
            int firstRow = rows.first().row();
            if(firstRow == 0){
                QItemSelectionModel * selectionModel = ui->listView->selectionModel();
                QModelIndexList rows = selectionModel->selectedRows();
                rows.removeAt(0);
                selectionModel->clearSelection();
                for(auto & index : rows){
                    selectionModel->select(index, QItemSelectionModel::Select);
                }
                ui->listView->setSelectionModel(selectionModel);
                if(ui->listView->selectionModel()->selectedRows().size() > 0)
                    firstRow = ui->listView->selectionModel()->selectedRows().first().row();
                else return;
            }
            rows = ui->listView->selectionModel()->selectedRows();
            std::sort(rows.begin(), rows.end());
            ui->listView->clearSelection();
            switch(type){
            case SeasonJumpersItems:{
                SeasonJumpersListModel * jumpersListModel = dynamic_cast<SeasonJumpersListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(jumpersListModel->index(index.row() - 1));
                for(auto & index : rows)
                    swapVectorItems(jumpersListModel->getSeasonJumpers(), index.row(), index.row() - 1);
                emit jumpersListModel->dataChanged(jumpersListModel->index(firstRow), jumpersListModel->index(jumpersListModel->rowCount() - 1));
                break;
            }
            case SeasonHillsItems:{
                SeasonHillsListModel * hillsListModel = dynamic_cast<SeasonHillsListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(hillsListModel->index(index.row() - 1));
                for(auto & index : rows)
                    swapVectorItems(hillsListModel->getSeasonHills(), index.row(), index.row() - 1);
                emit hillsListModel->dataChanged(hillsListModel->index(firstRow), hillsListModel->index(hillsListModel->rowCount() - 1));
                break;
            }
            case JumperItems:{
                JumpersListModel * jumpersListModel = dynamic_cast<JumpersListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(jumpersListModel->index(index.row() - 1));
                for(auto & index : rows)
                    swapVectorItems(jumpersListModel->getJumpersVectorPointer(), index.row(), index.row() - 1);
                emit jumpersListModel->dataChanged(jumpersListModel->index(firstRow), jumpersListModel->index(jumpersListModel->rowCount() - 1));
                break;
            }
            case HillItems:{
                HillsListModel * hillsListModel = dynamic_cast<HillsListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(hillsListModel->index(index.row() - 1));
                for(auto & index : rows)
                    swapVectorItems(hillsListModel->getHillsVectorPointer(), index.row(), index.row() - 1);
                emit hillsListModel->dataChanged(hillsListModel->index(firstRow), hillsListModel->index(hillsListModel->rowCount() - 1));
                break;
            }
            case CompetitionRulesItems:{
                CompetitionRulesListModel * competitionRulesListModel = dynamic_cast<CompetitionRulesListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(competitionRulesListModel->index(index.row() - 1));
                for(auto & index : rows)
                    swapVectorItems(competitionRulesListModel->getCompetitonRulesVectorPointer(), index.row(), index.row() - 1);
                emit competitionRulesListModel->dataChanged(competitionRulesListModel->index(firstRow), competitionRulesListModel->index(competitionRulesListModel->rowCount() - 1));
                break;
            }
            case ClassificationItems:{
                ClassificationsListModel * classificationsListModel = dynamic_cast<ClassificationsListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(classificationsListModel->index(index.row() - 1));
                for(auto & index : rows)
                    swapVectorItems(classificationsListModel->getClassificationsVectorPointer(), index.row(), index.row() - 1);
                emit classificationsListModel->dataChanged(classificationsListModel->index(firstRow), classificationsListModel->index(classificationsListModel->rowCount() - 1));
                break;
            }
            case PointsForPlacesPresetsItems:{
                PointsForPlacesPresetsListModel * model = dynamic_cast<PointsForPlacesPresetsListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(model->index(index.row() - 1));
                for(auto & index : rows)
                    swapVectorItems(model->getPresetsVectorPointer(), index.row(), index.row() - 1);
                emit model->dataChanged(model->index(firstRow), model->index(model->rowCount() - 1));
                break;
            }
            case JumpersListsItems:{
                JumpersListsListModel * model = dynamic_cast<JumpersListsListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(model->index(index.row() - 1));
                for(auto & index : rows)
                    swapVectorItems(model->getJumpersLists(), index.row(), index.row() - 1);
                emit model->dataChanged(model->index(firstRow), model->index(model->rowCount() - 1));
                break;
            }
            case CalendarItems:{
                CalendarsListModel * model = dynamic_cast<CalendarsListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(model->index(index.row() - 1));
                for(auto & index : rows)
                    swapVectorItems(model->getCalendars(), index.row(), index.row() - 1);
                emit model->dataChanged(model->index(firstRow), model->index(model->rowCount() - 1));
                break;
            }
            default: break;
            }
            emit up();
        }
    }
}

void DatabaseItemsListView::onDownActionTriggered()
{
    if(allowMoving){
        int rowCount = listModel->rowCount();

        QModelIndexList rows = ui->listView->selectionModel()->selectedRows();
        if(rows.size() > 0){
            int lastRow = rows.last().row();
            if(lastRow == rowCount - 1){
                QItemSelectionModel * selectionModel = ui->listView->selectionModel();
                QModelIndexList rows = selectionModel->selectedRows();
                rows.removeLast();
                selectionModel->clearSelection();
                for(auto & index : rows){
                    selectionModel->select(index, QItemSelectionModel::Select);
                }
                ui->listView->setSelectionModel(selectionModel);
                if(ui->listView->selectionModel()->selectedRows().size() > 0)
                    lastRow = ui->listView->selectionModel()->selectedRows().last().row();
                else return;
            }
            rows = ui->listView->selectionModel()->selectedRows();
            std::sort(rows.begin(), rows.end(), [](const QModelIndex & index1, const QModelIndex & index2){
                return index1.row() > index2.row();
            });
            ui->listView->clearSelection();
            switch(type){
            case SeasonJumpersItems:{
                SeasonJumpersListModel * jumpersListModel = dynamic_cast<SeasonJumpersListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(jumpersListModel->index(index.row() + 1));
                for(auto & index : rows)
                    swapVectorItems(jumpersListModel->getSeasonJumpers(), index.row(), index.row() + 1);
                emit jumpersListModel->dataChanged(jumpersListModel->index(lastRow), jumpersListModel->index(0));
                break;
            }
            case SeasonHillsItems:{
               SeasonHillsListModel * hillsListModel = dynamic_cast<SeasonHillsListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(hillsListModel->index(index.row() + 1));
                for(auto & index : rows)
                    swapVectorItems(hillsListModel->getSeasonHills(), index.row(), index.row() + 1);
                emit hillsListModel->dataChanged(hillsListModel->index(lastRow), hillsListModel->index(0));
                break;
            }
            case JumperItems:{
                JumpersListModel * jumpersListModel = dynamic_cast<JumpersListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(jumpersListModel->index(index.row() + 1));
                for(auto & index : rows)
                    swapVectorItems(jumpersListModel->getJumpersVectorPointer(), index.row(), index.row() + 1);
                emit jumpersListModel->dataChanged(jumpersListModel->index(lastRow), jumpersListModel->index(0));
                break;
            }
            case HillItems:{
                HillsListModel * hillsListModel = dynamic_cast<HillsListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(hillsListModel->index(index.row() + 1));
                for(auto & index : rows)
                    swapVectorItems(hillsListModel->getHillsVectorPointer(), index.row(), index.row() + 1);
                emit hillsListModel->dataChanged(hillsListModel->index(lastRow), hillsListModel->index(0));
                break;
            }
            case CompetitionRulesItems:{
                CompetitionRulesListModel * competitionRulesListModel = dynamic_cast<CompetitionRulesListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(competitionRulesListModel->index(index.row() + 1));
                for(auto & index : rows)
                    swapVectorItems(competitionRulesListModel->getCompetitonRulesVectorPointer(), index.row(), index.row() + 1);
                emit competitionRulesListModel->dataChanged(competitionRulesListModel->index(lastRow), competitionRulesListModel->index(0));
                break;
            }
            case ClassificationItems:{
                ClassificationsListModel * classificationsListModel = dynamic_cast<ClassificationsListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(classificationsListModel->index(index.row() + 1));
                for(auto & index : rows)
                    swapVectorItems(classificationsListModel->getClassificationsVectorPointer(), index.row(), index.row() + 1);
                emit classificationsListModel->dataChanged(classificationsListModel->index(lastRow), classificationsListModel->index(0));
                break;
            }
            case PointsForPlacesPresetsItems:{
                PointsForPlacesPresetsListModel * model = dynamic_cast<PointsForPlacesPresetsListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(model->index(index.row() + 1));
                for(auto & index : rows)
                    swapVectorItems(model->getPresetsVectorPointer(), index.row(), index.row() + 1);
                emit model->dataChanged(model->index(lastRow), model->index(0));
                break;
            }
            case JumpersListsItems:{
                JumpersListsListModel * model = dynamic_cast<JumpersListsListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(model->index(index.row() + 1));
                for(auto & index : rows)
                    swapVectorItems(model->getJumpersLists(), index.row(), index.row() + 1);
                emit model->dataChanged(model->index(lastRow), model->index(0));
                break;
            }
            case CalendarItems:{
                CalendarsListModel * model = dynamic_cast<CalendarsListModel *>(listModel);
                for(auto & index : rows)
                    ui->listView->setCurrentIndex(model->index(index.row() + 1));
                for(auto & index : rows)
                    swapVectorItems(model->getCalendars(), index.row(), index.row() + 1);
                emit model->dataChanged(model->index(lastRow), model->index(0));
                break;
            }
            default: break;
            }
            emit down();
        }
    }
}

QVector<CompetitionRules> *DatabaseItemsListView::getCompetitionRules() const
{
    return competitionRules;
}

void DatabaseItemsListView::setCompetitionRules(QVector<CompetitionRules> *newCompetitionRules)
{
    competitionRules = newCompetitionRules;
}

QListView *DatabaseItemsListView::getListView()
{
    return ui->listView;
}

QVector<Hill> *DatabaseItemsListView::getHills() const
{
    return hills;
}

void DatabaseItemsListView::setHills(QVector<Hill> *newHills)
{
    hills = newHills;
}

QVector<Jumper> *DatabaseItemsListView::getJumpers() const
{
    return jumpers;
}

void DatabaseItemsListView::setJumpers(QVector<Jumper> *newJumpers)
{
    jumpers = newJumpers;
}

bool DatabaseItemsListView::getAllowInserting() const
{
    return allowInserting;
}

void DatabaseItemsListView::setAllowInserting(bool newAllowInserting)
{
    allowInserting = newAllowInserting;
}
