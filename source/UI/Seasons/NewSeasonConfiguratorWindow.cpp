#include "NewSeasonConfiguratorWindow.h"
#include "ui_NewSeasonConfiguratorWindow.h"

#include "../../global/GlobalDatabase.h"
#include "../../global/GlobalAppSettings.h"
#include "../../utilities/functions.h"
#include "../DatabaseEditor/DatabaseItemsListView.h"
#include "../ResponsiveWindowUtils.h"
#include <QMessageBox>
#include <QComboBox>
#include <QInputDialog>
#include <QDialog>
#include <QLabel>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSplitter>

namespace
{
QWidget *scrollableLayoutPanel(QLayout *layout, QWidget *parent)
{
    layout->setParent(nullptr);
    QWidget *content = new QWidget;
    content->setLayout(layout);
    content->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QScrollArea *scrollArea = new QScrollArea(parent);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidget(content);
    return scrollArea;
}
}

NewSeasonConfiguratorWindow::NewSeasonConfiguratorWindow(bool nextSeason, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSeasonConfiguratorDialog),
    compactLayout(false),
    actualJumperIndex(0),
    actualHillIndex(0),
    actualRulesIndex(0),
    classificationsListViewActualElement(-1),
    nextSeason(nextSeason)
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
    createResponsivePageSplitters();
    updateResponsiveLayout(width());
    ResponsiveWindowUtils::manageWindowGeometry(this, "NewSeasonConfiguratorWindow");
}

NewSeasonConfiguratorWindow::~NewSeasonConfiguratorWindow()
{
    delete ui;
}

void NewSeasonConfiguratorWindow::createResponsivePageSplitters()
{
    ui->horizontalLayout->removeItem(ui->verticalLayout_jumpersList);
    ui->horizontalLayout->removeItem(ui->verticalLayout_jumperEditor);
    ui->horizontalLayout->removeItem(ui->verticalLayout_5);
    while(QLayoutItem *item = ui->horizontalLayout->takeAt(0))
        delete item;
    QSplitter *jumpersSplitter = new QSplitter(Qt::Horizontal, ui->page_jumpers);
    jumpersSplitter->setObjectName("splitter_newSeasonJumpers");
    jumpersSplitter->addWidget(scrollableLayoutPanel(ui->verticalLayout_jumpersList, jumpersSplitter));
    jumpersSplitter->addWidget(scrollableLayoutPanel(ui->verticalLayout_jumperEditor, jumpersSplitter));
    jumpersSplitter->addWidget(scrollableLayoutPanel(ui->verticalLayout_5, jumpersSplitter));
    ui->horizontalLayout->addWidget(jumpersSplitter);
    pageSplitters.append(jumpersSplitter);

    ui->horizontalLayout_2->removeItem(ui->verticalLayout_hillsList);
    ui->horizontalLayout_2->removeItem(ui->verticalLayout_hillEditor);
    ui->horizontalLayout_2->removeWidget(ui->textEdit_2);
    while(QLayoutItem *item = ui->horizontalLayout_2->takeAt(0))
        delete item;
    QSplitter *hillsSplitter = new QSplitter(Qt::Horizontal, ui->page_hills);
    hillsSplitter->setObjectName("splitter_newSeasonHills");
    hillsSplitter->addWidget(scrollableLayoutPanel(ui->verticalLayout_hillsList, hillsSplitter));
    hillsSplitter->addWidget(scrollableLayoutPanel(ui->verticalLayout_hillEditor, hillsSplitter));
    hillsSplitter->addWidget(ui->textEdit_2);
    ui->horizontalLayout_2->addWidget(hillsSplitter);
    pageSplitters.append(hillsSplitter);

    ui->horizontalLayout_4->removeItem(ui->verticalLayout_rulesList);
    ui->horizontalLayout_4->removeItem(ui->verticalLayout_rulesEditor);
    while(QLayoutItem *item = ui->horizontalLayout_4->takeAt(0))
        delete item;
    QSplitter *rulesSplitter = new QSplitter(Qt::Horizontal, ui->page_competitionRules);
    rulesSplitter->setObjectName("splitter_newSeasonRules");
    rulesSplitter->addWidget(scrollableLayoutPanel(ui->verticalLayout_rulesList, rulesSplitter));
    rulesSplitter->addWidget(scrollableLayoutPanel(ui->verticalLayout_rulesEditor, rulesSplitter));
    ui->horizontalLayout_4->addWidget(rulesSplitter);
    pageSplitters.append(rulesSplitter);

    for(QSplitter *splitter : pageSplitters) {
        splitter->setChildrenCollapsible(false);
        for(int index = 0; index < splitter->count(); ++index)
            splitter->setStretchFactor(index, index == 1 ? 2 : 1);
    }
}

void NewSeasonConfiguratorWindow::updateResponsiveLayout(int windowWidth)
{
    const bool useCompactLayout = windowWidth < 1000;
    if(useCompactLayout == compactLayout && !pageSplitters.isEmpty() &&
            pageSplitters.first()->orientation() == (compactLayout ? Qt::Vertical : Qt::Horizontal))
        return;

    compactLayout = useCompactLayout;
    for(QSplitter *splitter : pageSplitters) {
        splitter->setOrientation(compactLayout ? Qt::Vertical : Qt::Horizontal);
        if(compactLayout) {
            QVector<int> sizes(splitter->count(), qMax(130, height() / splitter->count()));
            splitter->setSizes(sizes.toList());
        }
        else {
            QList<int> sizes;
            sizes << qMax(220, windowWidth / 4) << qMax(420, windowWidth / 2);
            if(splitter->count() == 3)
                sizes << qMax(180, windowWidth / 4);
            splitter->setSizes(sizes);
        }
    }
}

void NewSeasonConfiguratorWindow::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    updateResponsiveLayout(event->size().width());
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
