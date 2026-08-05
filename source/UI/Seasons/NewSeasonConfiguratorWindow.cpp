#include "NewSeasonConfiguratorWindow.h"
#include "ui_NewSeasonConfiguratorWindow.h"

#include "../../global/GlobalDatabase.h"
#include "../../global/GlobalAppSettings.h"
#include "../../utilities/functions.h"
#include "../DatabaseEditor/DatabaseItemsListView.h"
#include <QMessageBox>
#include <QComboBox>
#include <QInputDialog>
#include <QDialog>
#include <QLabel>

NewSeasonConfiguratorWindow::NewSeasonConfiguratorWindow(bool nextSeason, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSeasonConfiguratorDialog),
    classificationsListViewActualElement(-1),
    actualHillIndex(0),
    actualJumperIndex(0),
    actualRulesIndex(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    ui->toolBox->setCurrentIndex(0);
    for(auto & globalJumper : GlobalDatabase::get()->getEditableGlobalJumpers())
        jumpers.push_back(new Jumper(globalJumper));
    jumpersListView = new DatabaseItemsListView(DatabaseItemsListView::SeasonJumpersItems, true, !nextSeason, true, this);
    jumpersListView->setSeasonJumpers(&jumpers);
    jumpersListView->setupListModel();
    jumpersListView->selectOnlyFirstRow();
    ui->verticalLayout_jumpersList->addWidget(jumpersListView);

    jumperEditor = new JumperEditorWidget();
    jumperEditor->setParent(this);
    ui->verticalLayout_jumperEditor->addWidget(jumperEditor);
    if(!jumpers.isEmpty())
    {
        jumperEditor->setJumper(jumpers.first());
        jumperEditor->fillJumperInputs();
    }
    connect(jumpersListView, &DatabaseItemsListView::listViewDoubleClicked, this, [this](const QModelIndex &index){
        jumperEditor->show();
        jumperEditor->setJumper(jumpers[index.row()]);
        jumperEditor->fillJumperInputs();
        actualJumperIndex = index.row();
    });
    connect(jumperEditor, &JumperEditorWidget::submitted, this, [this](){
        if(actualJumperIndex > (-1) && actualJumperIndex < jumpers.count()){
            *jumpers[actualJumperIndex] = jumperEditor->getJumperFromWidgetInput();
            emit jumpersListView->getListModel()->dataChanged(jumpersListView->getListModel()->index(actualJumperIndex), jumpersListView->getListModel()->index(actualJumperIndex));
        }
    });

    for(auto & hill : GlobalDatabase::get()->getEditableGlobalHills())
        hills.push_back(new Hill(hill));
    hillsListView = new DatabaseItemsListView(DatabaseItemsListView::SeasonHillsItems, true, !nextSeason, true, this);
    hillsListView->setSeasonHills(&hills);
    hillsListView->setupListModel();
    hillsListView->selectOnlyFirstRow();
    ui->verticalLayout_hillsList->addWidget(hillsListView);

    hillEditor = new HillEditorWidget(this);
    ui->verticalLayout_hillEditor->addWidget(hillEditor);
    if(!hills.isEmpty())
    {
        hillEditor->setHill(hills.first());
        hillEditor->fillHillInputs();
    }

    connect(hillsListView, &DatabaseItemsListView::listViewDoubleClicked, this, [this](const QModelIndex &index){
        hillEditor->show();
        hillEditor->setHill(hills[index.row()]);
        hillEditor->fillHillInputs();
        actualHillIndex = index.row();
    });
    connect(hillsListView, &DatabaseItemsListView::insert, this, [this](){
        calendar.fixCompetitionsHills(&hills, hills.first());
    });
    connect(hillsListView, &DatabaseItemsListView::remove, this, [this](){
        calendar.fixCompetitionsHills(&hills, hills.first());
    });
    connect(hillEditor, &HillEditorWidget::submitted, this, [this](){
        if(actualHillIndex > (-1) && actualHillIndex < hills.count()){
            *hills[actualHillIndex] = hillEditor->getHillFromWidgetInput();
            emit hillsListView->getListModel()->dataChanged(hillsListView->getListModel()->index(actualHillIndex), hillsListView->getListModel()->index(actualHillIndex));
        }
    });

    competitionsRules = GlobalDatabase::get()->getEditableCompetitionRules();
    rulesListView = new DatabaseItemsListView(DatabaseItemsListView::CompetitionRulesItems, true, true, true, this);
    rulesListView->setCompetitionRules(&competitionsRules);
    rulesListView->setupListModel();
    rulesListView->selectOnlyFirstRow();
    ui->verticalLayout_rulesList->addWidget(rulesListView);

    rulesEditor = new CompetitionRulesEditorWidget(this);
    ui->verticalLayout_rulesEditor->addWidget(rulesEditor);
    if(!competitionsRules.isEmpty())
    {
        rulesEditor->setCompetitionRules(&competitionsRules.first());
        rulesEditor->fillCompetitionRulesInputs();
    }

    connect(rulesListView, &DatabaseItemsListView::listViewDoubleClicked, this, [this](const QModelIndex &index){
        rulesEditor->show();
        rulesEditor->setCompetitionRules(&competitionsRules[index.row()]);
        rulesEditor->fillCompetitionRulesInputs();
        actualRulesIndex = index.row();
    });
    connect(rulesEditor, &CompetitionRulesEditorWidget::submitted, this, [this](){
        if(actualRulesIndex > (-1) && actualRulesIndex < competitionsRules.count()){
            competitionsRules[actualRulesIndex] = rulesEditor->getCompetitionRulesFromWidgetInputs();
            emit rulesListView->getListModel()->dataChanged(rulesListView->getListModel()->index(actualRulesIndex), rulesListView->getListModel()->index(actualRulesIndex));
        }
    });

    connect(hillsListView, &DatabaseItemsListView::remove, this, [this](){
        calendar.fixCompetitionsHills(&hills, hills.first());
        emit calendarTableModel->dataChanged(calendarTableModel->index(0, 0), calendarTableModel->index(calendarTableModel->rowCount() - 1, 6));
    });
}

NewSeasonConfiguratorWindow::~NewSeasonConfiguratorWindow()
{
    delete ui;
}

void NewSeasonConfiguratorWindow::showCalendarEditorHelp()
{
    if(GlobalAppSettings::get()->getShowCalendarEditorHelp() == true){
        QMessageBox box;
        box.setWindowTitle(tr("Edytor kalendarzy"));
        box.setText(tr("Aby dodać konkurs do kalendarza użyj \"Ctrl + A\". Żeby usunąć zawody użyj \"Ctrl + D\".\n Aby przesuwać konkursy, użyj Ctrl i strzałek na klawiaturze.\nAby edytować informacje o konkursie zaznacz w tabeli wiersze informacji o konkursie które chcesz edytować (Wszystkie w tej samej kolumnie) i naciśnij \"Ctrl + E\".\nMożna edytować kilka konkursów na raz (Jedynym wyjątakiem jest kolumna \"Awans (Konkurs)\" gdzie zaznaczyć można tylko jeden wiersz."));
        box.setStandardButtons(QMessageBox::Ok);
        QAbstractButton *pButtonDontShow = box.addButton(tr("Nie pokazuj ponownie"), QMessageBox::ActionRole);
        box.exec();
        if(box.clickedButton() == pButtonDontShow)
        {
            GlobalAppSettings::get()->setShowCalendarEditorHelp(false);
            GlobalAppSettings::get()->writeToJson();
        }
    }
}

void NewSeasonConfiguratorWindow::on_pushButton_submit_clicked()
{
    accept();
}

QVector<CompetitionRules> &NewSeasonConfiguratorWindow::getCompetitionsRulesReference()
{
    return competitionsRules;
}

void NewSeasonConfiguratorWindow::setCompetitionsRules(const QVector<CompetitionRules> &newCompetitionsRules)
{
    competitionsRules = newCompetitionsRules;
}

QToolBox *NewSeasonConfiguratorWindow::getToolBox()
{
    return ui->toolBox;
}

QVector<Hill *> NewSeasonConfiguratorWindow::getHills() const
{
    return hills;
}

QVector<Hill *> NewSeasonConfiguratorWindow::getHillsReference()
{
    return hills;
}

void NewSeasonConfiguratorWindow::setHills(const QVector<Hill *> &newHills)
{
    hills = newHills;
}

ClassificationEditorWidget *NewSeasonConfiguratorWindow::getClassificationEditor() const
{
    return classificationEditor;
}

void NewSeasonConfiguratorWindow::setClassificationEditor(ClassificationEditorWidget *newClassificationEditor)
{
    classificationEditor = newClassificationEditor;
}

CalendarEditorTableModel *NewSeasonConfiguratorWindow::getCalendarTableModel() const
{
    return calendarTableModel;
}

void NewSeasonConfiguratorWindow::setCalendarTableModel(CalendarEditorTableModel *newCalendarTableModel)
{
    calendarTableModel = newCalendarTableModel;
}

SeasonCalendar NewSeasonConfiguratorWindow::getCalendar() const
{
    return calendar;
}

void NewSeasonConfiguratorWindow::setCalendar(const SeasonCalendar &newCalendar)
{
    calendar = newCalendar;
}

CalendarEditorWidget *NewSeasonConfiguratorWindow::getCalendarEditor() const
{
    return calendarEditor;
}

void NewSeasonConfiguratorWindow::setCalendarEditor(CalendarEditorWidget *newCalendarEditor)
{
    calendarEditor = newCalendarEditor;
}

QVector<Jumper *> NewSeasonConfiguratorWindow::getJumpers() const
{
    return jumpers;
}

QVector<Jumper *> &NewSeasonConfiguratorWindow::getJumpersReference()
{
    return jumpers;
}

void NewSeasonConfiguratorWindow::setJumpers(const QVector<Jumper *> &newJumpers)
{
    jumpers = newJumpers;
}

bool NewSeasonConfiguratorWindow::getNextSeason() const
{
    return nextSeason;
}

void NewSeasonConfiguratorWindow::setNextSeason(bool newNextSeason)
{
    nextSeason = newNextSeason;
}
