#include "DatabaseEditorWindow.h"
#include "../../athletes/AthleteStudioController.h"
#include "../../hills/HillStudioController.h"
#include "ui_DatabaseEditorWindow.h"

#include "../../global/GlobalDatabase.h"
#include "../../global/CountryFlagsManager.h"
#include "../../simulator/Hill.h"
#include "../EditorWidgets/JumperEditorWidget.h"
#include "../EditorWidgets/HillEditorWidget.h"
#include "../EditorWidgets/CompetitionRulesEditorWidget.h"
#include "../EditorWidgets/PointsForPlacesPresetEditorWidget.h"
#include "ListModels/JumpersListModel.h"
#include "ListModels/HillsListModel.h"
#include "ListModels/CompetitionRulesListModel.h"
#include "ListModels/PointsForPlacesPresetsListModel.h"
#include "DatabaseItemsListView.h"
#include "../ResponsiveWindowUtils.h"
#include <QFont>
#include <QScrollArea>
#include <QCloseEvent>
#include <QMessageBox>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QInputDialog>
#include <QFileDialog>
#include <QSaveFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSignalBlocker>
#include <QFormLayout>

namespace
{
// Keep detail inputs comfortable to scan on ultrawide displays. The surrounding
// scroll area still shrinks normally on smaller windows.
constexpr int editorMaximumWidth = 900;
}

DatabaseEditorWindow::DatabaseEditorWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseEditorWindow),
    packChoice(nullptr),
    jumperEditor(nullptr),
    hillEditor(nullptr),
    competitionRulesEditor(nullptr),
    actualElementType(0),
    actualElementIndex(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tabWidget_main->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setProperty("athleteStudio",QVariant::fromValue(static_cast<QObject *>(new AthleteStudioController(this))));
    setProperty("hillStudio",QVariant::fromValue(static_cast<QObject *>(new HillStudioController(this))));
    originalPacks = GlobalDatabase::get()->packsSnapshot();
    tempGlobalJumpers = GlobalDatabase::get()->getGlobalJumpers();
    tempGlobalHills = GlobalDatabase::get()->getGlobalHills();
    tempGlobalCompetitionRules = GlobalDatabase::get()->getGlobalCompetitionsRules();
    tempGlobalPointsForPlacesPresets = GlobalDatabase::get()->getGlobalPointsForPlacesPresets();

    jumperEditor = new JumperEditorWidget;
    jumperEditor->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    jumperEditor->setMaximumWidth(editorMaximumWidth);
    jumperEditor->hide();
    ui->verticalLayout_jumperEditor->addWidget(jumperEditor, 0, Qt::AlignHCenter | Qt::AlignTop);

    hillEditor = new HillEditorWidget();
    hillEditor->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    hillEditor->setMaximumWidth(editorMaximumWidth);
    hillEditor->hide();
    ui->verticalLayout_hillEditor->addWidget(hillEditor, 0, Qt::AlignHCenter | Qt::AlignTop);

    competitionRulesEditor = new CompetitionRulesEditorWidget();
    competitionRulesEditor->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    competitionRulesEditor->setMaximumWidth(editorMaximumWidth);
    competitionRulesEditor->hide();
    ui->verticalLayout_rulesEditor->addWidget(competitionRulesEditor, 0, Qt::AlignHCenter | Qt::AlignTop);

    pointsForPlacesPresetEditor = new PointsForPlacesPresetEditorWidget();
    pointsForPlacesPresetEditor->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    pointsForPlacesPresetEditor->setMaximumWidth(editorMaximumWidth);
    pointsForPlacesPresetEditor->hide();
    ui->verticalLayout_pointsForPlacesEditor->addWidget(pointsForPlacesPresetEditor, 0, Qt::AlignHCenter | Qt::AlignTop);

    jumpersListView = new DatabaseItemsListView(DatabaseItemsListView::JumperItems, true, true, true, this);
    jumpersListView->setJumpers(&GlobalDatabase::get()->getEditableGlobalJumpers());
    jumpersListView->setupListModel();
    jumpersListView->selectOnlyFirstRow();
    ui->verticalLayout_jumpersList->addWidget(jumpersListView);

    hillsListView = new DatabaseItemsListView(DatabaseItemsListView::HillItems, true, true, true, this);
    hillsListView->setHills(&GlobalDatabase::get()->getEditableGlobalHills());
    hillsListView->setupListModel();
    hillsListView->selectOnlyFirstRow();
    ui->verticalLayout_hillsList->addWidget(hillsListView);

    competitionRulesListView = new DatabaseItemsListView(DatabaseItemsListView::CompetitionRulesItems, true, true, true, this);
    competitionRulesListView->setCompetitionRules(&GlobalDatabase::get()->getEditableCompetitionRules());
    competitionRulesListView->setupListModel();
    competitionRulesListView->selectOnlyFirstRow();
    ui->verticalLayout_competitionRulesList->addWidget(competitionRulesListView);

    pointsForPlacesPresetsListView = new DatabaseItemsListView(DatabaseItemsListView::PointsForPlacesPresetsItems, true, true, true, this);
    pointsForPlacesPresetsListView->setPointsForPlacesPresets(&GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets());
    pointsForPlacesPresetsListView->setupListModel();
    pointsForPlacesPresetsListView->selectOnlyFirstRow();
    ui->verticalLayout_pointsForPlacesPresetsList->addWidget(pointsForPlacesPresetsListView);

    connect(ui->tabWidget_main, &QTabWidget::currentChanged, this, [this](){
        if (!saveOpenEditors()) {
            const QSignalBlocker block(ui->tabWidget_main);
            ui->tabWidget_main->setCurrentIndex(actualElementType);
            return;
        }
        setActualElementType(ui->tabWidget_main->currentIndex());
        refreshPackControls();
    });
    connect(this->jumperEditor, &JumperEditorWidget::submitted, this, &DatabaseEditorWindow::replaceJumperByJumperEditor);
    connect(this->hillEditor, &HillEditorWidget::submitted, this, &DatabaseEditorWindow::replaceHillByHillEditor);
    connect(this->competitionRulesEditor, &CompetitionRulesEditorWidget::submitted, this, &DatabaseEditorWindow::replaceCompetitionRuleByCompetitionRulesEditor);
    connect(this->pointsForPlacesPresetEditor, &PointsForPlacesPresetEditorWidget::submitted, this, &DatabaseEditorWindow::replacePointsForPlacesPresetByEditorObject);

    connect(this->jumpersListView, &DatabaseItemsListView::listViewDoubleClicked, this, &DatabaseEditorWindow::onJumpersListViewDoubleClicked);
    connect(this->hillsListView, &DatabaseItemsListView::listViewDoubleClicked, this, &DatabaseEditorWindow::onHillsListViewDoubleClicked);
    connect(this->competitionRulesListView, &DatabaseItemsListView::listViewDoubleClicked, this, &DatabaseEditorWindow::onCompetitionRulesListViewDoubleClicked);
    connect(this->pointsForPlacesPresetsListView, &DatabaseItemsListView::listViewDoubleClicked, this, &DatabaseEditorWindow::onPointsForPlacesPresetsListViewDoubleClicked);

    connect(this->jumpersListView, &DatabaseItemsListView::up, this, [this](){
        actualElementIndex--;
    });
    connect(this->hillsListView, &DatabaseItemsListView::up, this, [this](){
        actualElementIndex--;
    });
    connect(this->competitionRulesListView, &DatabaseItemsListView::up, this, [this](){
        actualElementIndex--;
    });
    connect(this->pointsForPlacesPresetsListView, &DatabaseItemsListView::up, this, [this](){
        actualElementIndex--;
    });
    connect(this->jumpersListView, &DatabaseItemsListView::down, this, [this](){
        actualElementIndex++;
    });
    connect(this->hillsListView, &DatabaseItemsListView::down, this, [this](){
        actualElementIndex++;
    });
    connect(this->competitionRulesListView, &DatabaseItemsListView::down, this, [this](){
        actualElementIndex++;
    });
    connect(this->pointsForPlacesPresetsListView, &DatabaseItemsListView::down, this, [this](){
        actualElementIndex++;
    });
    setupPackControls();
    for (auto *list : {jumpersListView, hillsListView, competitionRulesListView, pointsForPlacesPresetsListView}) {
        connect(list, &DatabaseItemsListView::remove, this, [this] { resetPackEditors(); });
        connect(list, &DatabaseItemsListView::insert, this, [this] { resetPackEditors(); });
        connect(list, &DatabaseItemsListView::up, this, [this] { resetPackEditors(); });
        connect(list, &DatabaseItemsListView::down, this, [this] { resetPackEditors(); });
    }
    ResponsiveWindowUtils::relaxTextConstraints(this);
}

DatabaseEditorWindow::~DatabaseEditorWindow()
{
    delete ui;
}

void DatabaseEditorWindow::replaceJumperByJumperEditor()
{
    const int actualElementIndex = jumperEditor->property("packRow").toInt();
    if (actualElementIndex < 0) return;
    if(actualElementIndex < GlobalDatabase::get()->getEditableGlobalJumpers().count()){
        GlobalDatabase::get()->getEditableGlobalJumpers().replace(actualElementIndex, jumperEditor->getJumperFromWidgetInput());
        jumperEditor->setJumper(&GlobalDatabase::get()->getEditableGlobalJumpers()[actualElementIndex]);
        JumpersListModel * jumpersListModel = dynamic_cast<JumpersListModel *>(jumpersListView->getListModel());
        emit jumpersListModel->dataChanged(jumpersListModel->index(actualElementIndex), jumpersListModel->index(actualElementIndex));
    }
}

void DatabaseEditorWindow::replaceHillByHillEditor()
{
    const int actualElementIndex = hillEditor->property("packRow").toInt();
    if (actualElementIndex < 0) return;
    if(actualElementIndex < GlobalDatabase::get()->getEditableGlobalHills().count()){
        GlobalDatabase::get()->getEditableGlobalHills().replace(actualElementIndex, hillEditor->getHillFromWidgetInput());
        hillEditor->setHill(&GlobalDatabase::get()->getEditableGlobalHills()[actualElementIndex]);
        HillsListModel * hillsListModel = dynamic_cast<HillsListModel *>(hillsListView->getListModel());
        emit hillsListModel->dataChanged(hillsListModel->index(actualElementIndex), hillsListModel->index(actualElementIndex));
    }
}

void DatabaseEditorWindow::replaceCompetitionRuleByCompetitionRulesEditor()
{
    const int actualElementIndex = competitionRulesEditor->property("packRow").toInt();
    if (actualElementIndex < 0) return;
    if(actualElementIndex < GlobalDatabase::get()->getEditableCompetitionRules().count()){
        GlobalDatabase::get()->getEditableCompetitionRules().replace(actualElementIndex, competitionRulesEditor->getCompetitionRulesFromWidgetInputs());
        competitionRulesEditor->setCompetitionRules(&GlobalDatabase::get()->getEditableCompetitionRules()[actualElementIndex]);
        CompetitionRulesListModel * competitionRulesListModel = dynamic_cast<CompetitionRulesListModel *>(competitionRulesListView->getListModel());
        emit competitionRulesListModel->dataChanged(competitionRulesListModel->index(actualElementIndex), competitionRulesListModel->index(actualElementIndex));
    }
}

void DatabaseEditorWindow::replacePointsForPlacesPresetByEditorObject()
{
    const int actualElementIndex = pointsForPlacesPresetEditor->property("packRow").toInt();
    if (actualElementIndex < 0) return;
    if(actualElementIndex < GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets().count()){
        GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets().replace(actualElementIndex, pointsForPlacesPresetEditor->constructPresetFromInputs());
        pointsForPlacesPresetEditor->setPreset(&GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets()[actualElementIndex]);
        PointsForPlacesPresetsListModel * pointsForPlacesPresetsListModel = dynamic_cast<PointsForPlacesPresetsListModel *>(pointsForPlacesPresetsListView->getListModel());
        emit pointsForPlacesPresetsListModel->dataChanged(pointsForPlacesPresetsListModel->index(actualElementIndex), pointsForPlacesPresetsListModel->index(actualElementIndex));
    }
}

void DatabaseEditorWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox message;
    message.setStyleSheet("background-color: white; color: black;");
    QMessageBox::StandardButton button;
    button = message.question(this, tr("Wyjście z edytora bazy danych"), tr("Czy zapisać zmiany w bazie danych?"), QMessageBox::StandardButton::No | QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::Yes);
    if(button == QMessageBox::Yes)
    {
        if (!saveOpenEditors()) { event->ignore(); return; }
        if (!GlobalDatabase::get()->writePacks()) {
            QMessageBox::critical(this, tr("Paczki"), tr("Nie udało się zapisać paczek. Zmiany pozostają w edytorze."));
            event->ignore(); return;
        }
        GlobalDatabase::get()->writeJumpers();
        GlobalDatabase::get()->writeHills();
        GlobalDatabase::get()->writeCompetitionsRules();
        GlobalDatabase::get()->writePointsForPlacesPresets();
        event->accept();
    }
    else if(button == QMessageBox::Cancel)
    {
        event->ignore();
    }
    else if(button == QMessageBox::No)
    {
        resetPackEditors();
        QString error;
        GlobalDatabase::get()->restorePacks(originalPacks, error);
        GlobalDatabase::get()->setGlobalJumpers(tempGlobalJumpers);
        GlobalDatabase::get()->setGlobalHills(tempGlobalHills);
        GlobalDatabase::get()->setGlobalCompetitionsRules(tempGlobalCompetitionRules);
        GlobalDatabase::get()->setGlobalPointsForPlacesPresets(tempGlobalPointsForPlacesPresets);
        event->accept();
    }
    else
        qDebug()<<"invalid button in database editor close event";
}

void DatabaseEditorWindow::onJumpersListViewDoubleClicked(const QModelIndex &index)
{
    int row = index.row();
    actualElementIndex = row;
    jumperEditor->setProperty("packRow", row);
    if(jumperEditor->isHidden()){
        jumperEditor->show();
    }
    jumperEditor->setJumper(const_cast<Jumper *>(&GlobalDatabase::get()->getEditableGlobalJumpers().at(row)));
    jumperEditor->fillJumperInputs();
}

void DatabaseEditorWindow::onHillsListViewDoubleClicked(const QModelIndex &index)
{
    int row = index.row();
    actualElementIndex = row;
    hillEditor->setProperty("packRow", row);
    if(hillEditor->isHidden()){
        hillEditor->show();
    }
    hillEditor->setHill(const_cast<Hill *>(&GlobalDatabase::get()->getEditableGlobalHills().at(row)));
    hillEditor->fillHillInputs();
}

void DatabaseEditorWindow::onCompetitionRulesListViewDoubleClicked(const QModelIndex &index)
{
    int row = index.row();
    actualElementIndex = row;
    competitionRulesEditor->setProperty("packRow", row);
    if(competitionRulesEditor->isHidden()){
        competitionRulesEditor->show();
    }
    competitionRulesEditor->setCompetitionRules(const_cast<CompetitionRules *>(&GlobalDatabase::get()->getEditableCompetitionRules().at(row)));
    competitionRulesEditor->fillCompetitionRulesInputs();
}

void DatabaseEditorWindow::onPointsForPlacesPresetsListViewDoubleClicked(const QModelIndex &index)
{
    int row = index.row();
    actualElementIndex = row;
    pointsForPlacesPresetEditor->setProperty("packRow", row);
    if(pointsForPlacesPresetEditor->isHidden()){
        pointsForPlacesPresetEditor->show();
    }
    pointsForPlacesPresetEditor->reset();
    pointsForPlacesPresetEditor->setPreset(const_cast<PointsForPlacesPreset *>(&GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets().at(row)));
    pointsForPlacesPresetEditor->fill();
}

short DatabaseEditorWindow::getActualElementType() const
{
    return actualElementType;
}

void DatabaseEditorWindow::setActualElementType(short newActualElementType)
{
    actualElementType = newActualElementType;
}

QVector<CompetitionRules> DatabaseEditorWindow::getTempGlobalCompetitionRules() const
{
    return tempGlobalCompetitionRules;
}

void DatabaseEditorWindow::setTempGlobalCompetitionRules(const QVector<CompetitionRules> &newTempGlobalCompetitionRules)
{
    tempGlobalCompetitionRules = newTempGlobalCompetitionRules;
}

QVector<Hill> DatabaseEditorWindow::getTempGlobalHills() const
{
    return tempGlobalHills;
}

void DatabaseEditorWindow::setTempGlobalHills(const QVector<Hill> &newTempGlobalHills)
{
    tempGlobalHills = newTempGlobalHills;
}

QVector<Jumper> DatabaseEditorWindow::getTempGlobalJumpers() const
{
    return tempGlobalJumpers;
}

void DatabaseEditorWindow::setTempGlobalJumpers(const QVector<Jumper> &newTempGlobalJumpers)
{
    tempGlobalJumpers = newTempGlobalJumpers;
}

HillsListModel *DatabaseEditorWindow::getHillsListModel() const
{
    return hillsListModel;
}

void DatabaseEditorWindow::setHillsListModel(HillsListModel *newHillsListModel)
{
    hillsListModel = newHillsListModel;
}

int DatabaseEditorWindow::getActualElementIndex() const
{
    return actualElementIndex;
}

void DatabaseEditorWindow::setActualElementIndex(int newActualElementIndex)
{
    actualElementIndex = newActualElementIndex;
}

JumpersListModel *DatabaseEditorWindow::getJumpersListModel() const
{
    return jumpersListModel;
}

void DatabaseEditorWindow::setJumpersListModel(JumpersListModel *newJumpersListModel)
{
    jumpersListModel = newJumpersListModel;
}

void DatabaseEditorWindow::on_pushButton_sortByCountries_clicked()
{
    if(QMessageBox::question(this,tr("Sortowanie według krajów"),
        tr("Posortować paczkę według krajów? Zmieni to obecną kolejność elementów."),
        QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes)return;
    if (!saveOpenEditors()) return;
    if(ui->tabWidget_main->currentWidget() == ui->tab_jumpers)
    {
        QVector<Jumper> temp;
        QVector<QString> countries;
        for(auto & jumper : GlobalDatabase::get()->getEditableGlobalJumpers())
            if(countries.contains(jumper.getCountryCode()) == false)
                countries.push_back(jumper.getCountryCode());
        for(auto & country : countries)
        {
            for(auto & jumper : Team::getJumpersFilteredByCountryCode(GlobalDatabase::get()->getEditableGlobalJumpers(), country))
                temp.push_back(*jumper);
        }
        resetPackEditors();
        GlobalDatabase::get()->setGlobalJumpers(temp);
        jumpersListView->setJumpers(&GlobalDatabase::get()->getEditableGlobalJumpers());
        jumpersListView->setupListModel();
    }
    else if(ui->tabWidget_main->currentWidget() == ui->tab_hills)
    {
        QVector<Hill> temp;
        QVector<QString> countries;
        for(auto & hill : GlobalDatabase::get()->getEditableGlobalHills())
            if(countries.contains(hill.getCountryCode()) == false)
                countries.push_back(hill.getCountryCode());
        for(auto & country : countries)
        {
            for(auto & hill : Hill::getHillsListByCountryCode(GlobalDatabase::get()->getEditableGlobalHills(), country))
                temp.push_back(*hill);
        }
        resetPackEditors();
        GlobalDatabase::get()->setGlobalHills(temp);
        hillsListView->setHills(&GlobalDatabase::get()->getEditableGlobalHills());
        hillsListView->setupListModel();
    }
}

int DatabaseEditorWindow::packKind() const
{
    return ui->tabWidget_main->currentIndex() == 0 ? 0 : ui->tabWidget_main->currentIndex() == 1 ? 1 : 2;
}

void DatabaseEditorWindow::setupPackControls()
{
    auto *panel = new QWidget(this);
    panel->setObjectName("dataPackPanel");
    auto *layout = new QVBoxLayout(panel);
    auto *form = new QFormLayout;
    packChoice = new QComboBox(panel); packChoice->setObjectName("dataPackChoice");
    packName = new QLineEdit(panel); packName->setObjectName("dataPackName");
    form->addRow(tr("Aktywna paczka"), packChoice);
    form->addRow(tr("Nazwa paczki"), packName);
    layout->addLayout(form);
    ageSettings = new QWidget(panel);
    auto *ageLayout = new QFormLayout(ageSettings);
    ageFormat = new QComboBox(ageSettings); ageFormat->setObjectName("dataPackAgeFormat");
    ageFormat->addItems({tr("Data urodzenia"), tr("Wiek rocznikowy")});
    referenceYear = new QSpinBox(ageSettings); referenceYear->setObjectName("dataPackReferenceYear"); referenceYear->setRange(1, 9999);
    ageLayout->addRow(tr("Dane urodzenia w paczce"), ageFormat);
    ageLayout->addRow(tr("Rok odniesienia dla wieku"), referenceYear);
    layout->addWidget(ageSettings);
    auto *buttons = new QHBoxLayout;
    auto *create = new QPushButton(tr("Nowa paczka"), panel); create->setObjectName("createDataPack");
    auto *import = new QPushButton(tr("Importuj paczkę…"), panel); import->setObjectName("importDataPack");
    auto *exportPack = new QPushButton(tr("Eksportuj / udostępnij…"), panel); exportPack->setObjectName("exportDataPack");
    buttons->addWidget(create); buttons->addWidget(import); buttons->addWidget(exportPack);
    layout->addLayout(buttons);
    auto *hint = new QLabel(tr("Wybrana paczka jest używana w grze. Eksport tworzy jeden plik do udostępnienia; zasady i presety punktów są w tej samej paczce."), panel);
    hint->setWordWrap(true); layout->addWidget(hint);
    ui->verticalLayout->insertWidget(1, panel);
    connect(packChoice, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index < 0) return;
        if (!saveOpenEditors()) { refreshPackControls(); return; }
        resetPackEditors();
        GlobalDatabase::get()->selectPack(packChoice->itemData(index).toString());
        for (auto *list : {jumpersListView, hillsListView, competitionRulesListView, pointsForPlacesPresetsListView}) list->setupListModel();
        refreshPackControls();
    });
    connect(packName, &QLineEdit::textChanged, this, [this](const QString &text) {
        if (text.trimmed().isEmpty()) return;
        auto *p = GlobalDatabase::get()->activePack(DataPack::Kind(packKind()));
        p->name = text.trimmed();
        packChoice->setItemText(packChoice->currentIndex(), p->name);
    });
    connect(packName, &QLineEdit::editingFinished, this, [this] {
        if (packName->text().trimmed().isEmpty()) {
            const QSignalBlocker block(packName);
            packName->setText(GlobalDatabase::get()->activePack(DataPack::Kind(packKind()))->name);
        }
    });
    auto formatChanged = [this] {
        if (!saveOpenEditors()) { refreshPackControls(); return; }
        auto *p = GlobalDatabase::get()->activePack(DataPack::Jumpers);
        p->ageMode = ageFormat->currentIndex() == 1;
        p->referenceYear = referenceYear->value();
        jumperEditor->setAgeFormat(p->ageMode, p->referenceYear);
    };
    connect(ageFormat, qOverload<int>(&QComboBox::currentIndexChanged), this, formatChanged);
    connect(referenceYear, qOverload<int>(&QSpinBox::valueChanged), this, formatChanged);
    connect(create, &QPushButton::clicked, this, [this] {
        if (!saveOpenEditors()) return;
        bool ok;
        const auto name = QInputDialog::getText(this, tr("Nowa paczka"), tr("Nazwa paczki"), QLineEdit::Normal, {}, &ok).trimmed();
        if (!ok || name.isEmpty()) return;
        resetPackEditors();
        DataPack p; p.name = name; p.kind = DataPack::Kind(packKind());
        GlobalDatabase::get()->addPack(p);
        for (auto *list : {jumpersListView, hillsListView, competitionRulesListView, pointsForPlacesPresetsListView}) list->setupListModel();
        refreshPackControls();
    });
    connect(import, &QPushButton::clicked, this, [this] {
        if (!saveOpenEditors()) return;
        const auto path = QFileDialog::getOpenFileName(this, tr("Importuj paczkę"), {}, tr("Paczki Sj.Sim (*.json)"));
        if (path.isEmpty()) return;
        QFile file(path); QJsonParseError parse; QString error; DataPack p;
        const bool opened = file.open(QIODevice::ReadOnly);
        const auto doc = opened ? QJsonDocument::fromJson(file.readAll(), &parse) : QJsonDocument();
        if (!opened || parse.error != QJsonParseError::NoError || !DataPack::fromJson(doc.object(), p, error)) {
            QMessageBox::warning(this, tr("Import paczki"), error.isEmpty() ? tr("Nie udało się odczytać pliku JSON.") : error); return;
        }
        resetPackEditors();
        GlobalDatabase::get()->addPack(p);
        ui->tabWidget_main->setCurrentIndex(int(p.kind));
        for (auto *list : {jumpersListView, hillsListView, competitionRulesListView, pointsForPlacesPresetsListView}) list->setupListModel();
        refreshPackControls();
    });
    connect(exportPack, &QPushButton::clicked, this, [this] {
        if (!saveOpenEditors()) return;
        auto *db = GlobalDatabase::get(); db->syncActivePacks();
        const auto object = db->activePack(DataPack::Kind(packKind()))->toJson();
        const auto path = QFileDialog::getSaveFileName(this, tr("Eksportuj / udostępnij paczkę"), "paczka.json", tr("Paczki Sj.Sim (*.json)"));
        if (path.isEmpty()) return;
        QSaveFile file(path); const auto bytes = QJsonDocument(object).toJson(QJsonDocument::Indented);
        if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit())
            QMessageBox::critical(this, tr("Eksport paczki"), tr("Nie udało się zapisać pliku paczki."));
    });
    resetPackEditors();
    refreshPackControls();
}

void DatabaseEditorWindow::refreshPackControls()
{
    if (!packChoice) return;
    auto *db = GlobalDatabase::get();
    const auto kind = DataPack::Kind(packKind());
    const QSignalBlocker choiceBlock(packChoice), nameBlock(packName), formatBlock(ageFormat), yearBlock(referenceYear);
    packChoice->clear();
    for (const auto &p : db->getPacks()) if (p.kind == kind) packChoice->addItem(p.name, p.id);
    packChoice->setCurrentIndex(packChoice->findData(db->activePackId(kind)));
    auto *p = db->activePack(kind);
    packName->setText(p->name);
    ageSettings->setVisible(kind == DataPack::Jumpers);
    if (kind == DataPack::Jumpers) {
        ageFormat->setCurrentIndex(p->ageMode ? 1 : 0);
        referenceYear->setValue(p->referenceYear);
        jumperEditor->setAgeFormat(p->ageMode, p->referenceYear);
    }
}

void DatabaseEditorWindow::resetPackEditors()
{
    for (QWidget *editor : QVector<QWidget *>{jumperEditor, hillEditor, competitionRulesEditor, pointsForPlacesPresetEditor}) {
        editor->hide(); editor->setProperty("packRow", -1);
    }
    jumperEditor->setJumper(nullptr); hillEditor->setHill(nullptr);
    competitionRulesEditor->setCompetitionRules(nullptr); pointsForPlacesPresetEditor->setPreset(nullptr);
    actualElementIndex = -1;
}

bool DatabaseEditorWindow::saveOpenEditors()
{
    if (!jumperEditor->isHidden()) {
        if (!jumperEditor->validateBirthInput()) return false;
        replaceJumperByJumperEditor();
    }
    if (!hillEditor->isHidden()) replaceHillByHillEditor();
    if (!competitionRulesEditor->isHidden()) replaceCompetitionRuleByCompetitionRulesEditor();
    if (!pointsForPlacesPresetEditor->isHidden()) replacePointsForPlacesPresetByEditorObject();
    return true;
}
