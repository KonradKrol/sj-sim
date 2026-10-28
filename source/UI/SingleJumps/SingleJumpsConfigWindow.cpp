#include "SingleJumpsConfigWindow.h"
#include "ui_SingleJumpsConfigWindow.h"

#include "../EditorWidgets/JumperEditorWidget.h"
#include "../EditorWidgets/HillEditorWidget.h"
#include "../EditorWidgets/WindsGeneratorSettingsEditorWidget.h"
#include "../EditorWidgets/ConfigSelections.h"
#include "../ResponsiveWindowUtils.h"

#include "../../global/GlobalDatabase.h"
#include "../../global/CountryFlagsManager.h"
#include "../../global/GlobalSimulationSettings.h"

#include <QStringList>
#include <QStringListModel>
#include <QScrollArea>
#include <QCloseEvent>
#include <QMessageBox>
#include <QResizeEvent>
#include <QSplitter>
#include <cmath>
#include "../../simulation-v2/FieldWindMeasurement.h"
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QFormLayout>
#include "../EditorWidgets/SimulationProfileEditor.h"
#include "../EditorWidgets/WeatherSetupEditor.h"
#include "../EditorWidgets/WindMeasurementChoice.h"
#include <QSignalBlocker>
#include <QTimer>
#include "../../simulator/JumpSimulator.h"
#include "../../simulator/JumpManipulator.h"

SingleJumpsConfigWindow::SingleJumpsConfigWindow(QWidget *parent,const SimulationV2::CatalogPaths &paths) :
    QDialog(parent),
    ui(new Ui::SingleJumpsConfigWindow),
    contentSplitter(nullptr),
    compactLayout(false),
    catalog(paths)
{
    ui->setupUi(this);
    windMeasurementChoice=createWindMeasurementChoice(this);
    ui->verticalLayout_singleJumpsSettings->addWidget(new QLabel(tr("Pomiar wiatru"),this));
    ui->verticalLayout_singleJumpsSettings->addWidget(windMeasurementChoice);
    bindLegacyWindInputs(windMeasurementChoice,{ui->comboBox_windAverageCalculatingType,ui->comboBox_windCompensationDistanceEffect,ui->label_2,ui->label_3});
    ui->comboBox_windAverageCalculatingType->setItemText(WindsCalculator::Simple,tr("Uproszczone"));
    ui->comboBox_windAverageCalculatingType->setItemText(WindsCalculator::SimpleImproved,tr("Uproszczone z korektą kierunku"));
    ui->comboBox_windAverageCalculatingType->addItem(tr("Składowa wzdłuż zeskoku"));
    ui->comboBox_windAverageCalculatingType->setCurrentIndex(WindsCalculator::Original);
    ui->comboBox_windAverageCalculatingType->setToolTip(tr("Pomiar wzdłuż skoczni płynnie uwzględnia kierunek. Wiatr dokładnie boczny daje zerową składową przednią i tylną."));
    setWindowFlags(Qt::Window);

    ui->horizontalLayout_underTitle->removeWidget(ui->toolBox);
    ui->horizontalLayout_underTitle->removeItem(ui->verticalLayout_singleJumpsSettings);
    while(QLayoutItem *item = ui->horizontalLayout_underTitle->takeAt(0))
        delete item;

    QWidget *settingsPanel = new QWidget(this);
    ui->verticalLayout_singleJumpsSettings->setParent(nullptr);
    settingsPanel->setLayout(ui->verticalLayout_singleJumpsSettings);
    ui->verticalLayout_singleJumpsSettings->removeWidget(ui->pushButton_submit);

    QScrollArea *editorScrollArea = new QScrollArea(this);
    editorScrollArea->setWidgetResizable(true);
    editorScrollArea->setFrameShape(QFrame::NoFrame);
    editorScrollArea->setWidget(ui->toolBox);

    QScrollArea *settingsScrollArea = new QScrollArea(this);
    settingsScrollArea->setWidgetResizable(true);
    settingsScrollArea->setFrameShape(QFrame::NoFrame);
    settingsScrollArea->setWidget(settingsPanel);

    contentSplitter = new QSplitter(Qt::Horizontal, this);
    contentSplitter->setObjectName("splitter_singleJumpsConfiguration");
    contentSplitter->setChildrenCollapsible(false);
    contentSplitter->addWidget(editorScrollArea);
    contentSplitter->addWidget(settingsScrollArea);
    contentSplitter->setStretchFactor(0, 3);
    contentSplitter->setStretchFactor(1, 2);
    ui->horizontalLayout_underTitle->addWidget(contentSplitter);
    ui->verticalLayout_5->addWidget(ui->pushButton_submit, 0, Qt::AlignHCenter);
    ui->spinBox_dsqProbability->setValue(GlobalSimulationSettings::get()->getBaseDsqProbability());
    ui->pushButton_submit->setDefault(true);
    ui->pushButton_submit->setToolTip(tr("Rozpocznij symulację z aktualnymi ustawieniami (Enter)"));
    ui->spinBox_jumpsCount->setToolTip(tr("Liczba skoków do wykonania; minimum to 1"));
    auto *autoGate = new QPushButton(tr("Dobierz belkę"), this);
    autoGate->setObjectName("pushButton_autoGate");
    autoGate->setToolTip(tr("Oblicz belkę dla zawodnika, skoczni i ustawionego wiatru"));
    ui->horizontalLayout_9->addWidget(autoGate);
    connect(autoGate, &QPushButton::clicked, this, [this]{
        const int gate = suggestedGate();
        if(gate >= 0)ui->spinBox_gate->setValue(gate);
    });

    const QList<QWidget *> scalableInputs = {
        ui->spinBox_jumpsCount,
        ui->spinBox_dsqProbability,
        ui->comboBox_windCompensationDistanceEffect,
        ui->comboBox_windAverageCalculatingType,
        ui->doubleSpinBox_jumpsImportance,
        ui->spinBox_gate
    };
    for(QWidget *input : scalableInputs) {
        const int metricMinimum = qMax(88, input->fontMetrics().horizontalAdvance(QStringLiteral("000000")) + 32);
        input->setMinimumWidth(metricMinimum);
        input->setMaximumWidth(QWIDGETSIZE_MAX);
        input->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    const QList<QComboBox *> scalableComboBoxes = {
        ui->comboBox_windCompensationDistanceEffect,
        ui->comboBox_windAverageCalculatingType,
        ui->comboBox_jumpsImportancePreset
    };
    for(QComboBox *comboBox : scalableComboBoxes) {
        comboBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        comboBox->setMinimumContentsLength(8);
    }
    const QList<QLabel *> wrappingLabels = {
        ui->label_jumpsCount,
        ui->label_5,
        ui->label_3,
        ui->label_2,
        ui->label_6,
        ui->label
    };
    for(QLabel *label : wrappingLabels) {
        label->setWordWrap(true);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }
    const int submitHeight = qBound(48, ui->pushButton_submit->fontMetrics().height() + 18, 64);
    ui->pushButton_submit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    ui->pushButton_submit->setFixedHeight(submitHeight);
    ui->pushButton_submit->setMinimumWidth(250);
    ui->pushButton_submit->setMaximumWidth(480);

    jumperEditor = new JumperEditorWidget;
    jumperEditor->removeSubmitButton();

    hillEditor = new HillEditorWidget;
    hillEditor->removeSubmitButton();

    windsGeneratorSettingsEditor = new WindsGeneratorSettingsEditorWidget;
    windsGeneratorSettingsEditor->setRemovingSubmitButtons(true);
    windsGeneratorSettingsEditor->removeSubmitButton();
    windsGeneratorSettingsEditor->setKPoint(0);

    weatherSetup=new SimulationWeatherSetup(this);
    setProperty("weatherSetup",QVariant::fromValue(static_cast<QObject *>(weatherSetup)));
    athleteDraft = new SimulationProfileDraft(DataPack::Jumpers,this);
    hillDraft = new SimulationProfileDraft(DataPack::Hills,this);
    setProperty("athleteDraft",QVariant::fromValue(static_cast<QObject *>(athleteDraft)));
    setProperty("hillDraft",QVariant::fromValue(static_cast<QObject *>(hillDraft)));
    auto *sources=new QWidget(this);auto *sourceForm=new QFormLayout(sources);
    athletePack=new QComboBox(sources);athletePack->setObjectName("comboBox_athletePack");
    hillPack=new QComboBox(sources);hillPack->setObjectName("comboBox_hillPack");
    eventKind=new QComboBox(sources);eventKind->setObjectName("comboBox_simulationEvent");
    eventKind->addItems({tr("Trening"),tr("Seria próbna"),tr("Kwalifikacje"),tr("Konkurs")});eventKind->setCurrentIndex(3);
    sourceForm->addRow(tr("Paczka zawodników"),athletePack);sourceForm->addRow(tr("Paczka skoczni"),hillPack);
    sourceForm->addRow(tr("Rodzaj skoku"),eventKind);
    catalogStatus=new QLabel(tr("Wczytywanie paczek…"),this);catalogStatus->setObjectName("label_catalogStatus");catalogStatus->setWordWrap(true);
    ui->verticalLayout_singleJumpsSettings->insertWidget(0,sources);
    ui->verticalLayout_singleJumpsSettings->insertWidget(1,catalogStatus);
    auto *catalogDetails=new QPushButton(tr("Szczegóły wczytywania"),this);catalogDetails->setObjectName("pushButton_catalogDetails");catalogDetails->hide();
    ui->verticalLayout_singleJumpsSettings->insertWidget(2,catalogDetails);
    connect(catalogDetails,&QPushButton::clicked,this,[this]{
        QMessageBox details(QMessageBox::Information,tr("Wczytywanie paczek"),tr("Uwagi: %1").arg(snapshot.issues.size()),QMessageBox::Ok,this);
        details.setDetailedText(snapshot.issues.join("\n"));details.exec();
    });
    ui->pushButton_submit->setEnabled(false);
    connect(athletePack,QOverload<int>::of(&QComboBox::currentIndexChanged),this,[this]{if(!filling)fillPack(DataPack::Jumpers);});
    connect(hillPack,QOverload<int>::of(&QComboBox::currentIndexChanged),this,[this]{if(!filling)fillPack(DataPack::Hills);});
    connect(athleteDraft,&SimulationProfileDraft::changed,this,&SingleJumpsConfigWindow::updateReadiness);
    connect(hillDraft,&SimulationProfileDraft::changed,this,[this]{
        if(hillDraft->valid())weatherSetup->setHill(*configuredHill().getSimulationProfile());
        else weatherSetup->clearHill();
        if(hillDraft->valid() && windsGeneratorSettingsEditor->getKPoint()!=configuredHill().getKPoint()) {
            windsGeneratorSettingsEditor->setKPoint(configuredHill().getKPoint());
            windsGeneratorSettingsEditor->setWindGenerationSettings(&defaultWinds);
            windsGeneratorSettingsEditor->fillWindGenerationSettingsByDefault();windsGeneratorSettingsEditor->fillSettingsInputs();
        }
        updateReadiness();
    });
    ui->verticalLayout_jumperContent->addWidget(jumperEditor);
    ui->verticalLayout_hillContent->addWidget(hillEditor);
    ui->verticalLayout_windsGeneratorSettings->addWidget(windsGeneratorSettingsEditor);
    windsGeneratorSettingsEditor->hide();
    ui->verticalLayout_windsGeneratorSettings->addWidget(createWeatherSetupEditor(weatherSetup,this));
    connect(hillEditor, &HillEditorWidget::KPointInputChanged, this, [this](){
        windsGeneratorSettingsEditor->setKPoint(hillEditor->getKPointFromInput());
        windsGeneratorSettingsEditor->setWindGenerationSettings(&defaultWinds);
        windsGeneratorSettingsEditor->fillWindGenerationSettingsByDefault();
        windsGeneratorSettingsEditor->fillSettingsInputs();
    });

    ui->toolBox->setCurrentIndex(0);

    // Preserve the legacy components, but edit only parameters used by the new engine.
    jumperEditor->hide();hillEditor->hide();
    // The retained Widgets frontend also edits the new model, never ignored old skills.
    ui->verticalLayout_jumperContent->addWidget(createSimulationProfileEditor(athleteDraft,false,this));
    ui->verticalLayout_hillContent->addWidget(createSimulationProfileEditor(hillDraft,true,this));
    ui->label_6->hide();
    ui->checkBox_changeableWind->hide();
    ui->doubleSpinBox_jumpsImportance->setVisible(false);ui->comboBox_jumpsImportancePreset->setVisible(false);
    QTimer::singleShot(0,this,&SingleJumpsConfigWindow::loadCatalog);

    auto *gateLabel=new QLabel(this);gateLabel->setObjectName("label_suggestedGate");gateLabel->setWordWrap(true);
    ui->verticalLayout_singleJumpsSettings->addWidget(gateLabel);
    auto *gateTimer=new QTimer(this);gateTimer->setSingleShot(true);gateTimer->setInterval(40);
    connect(gateTimer,&QTimer::timeout,this,&SingleJumpsConfigWindow::updateGateSuggestion);
    connect(weatherSetup,&SimulationWeatherSetup::changed,gateTimer,QOverload<>::of(&QTimer::start));
    connect(athleteDraft,&SimulationProfileDraft::changed,gateTimer,QOverload<>::of(&QTimer::start));
    connect(hillDraft,&SimulationProfileDraft::changed,gateTimer,QOverload<>::of(&QTimer::start));
    connect(eventKind,QOverload<int>::of(&QComboBox::currentIndexChanged),gateTimer,QOverload<>::of(&QTimer::start));
    connect(ui->spinBox_gate,QOverload<int>::of(&QSpinBox::valueChanged),gateTimer,QOverload<>::of(&QTimer::start));

    const auto updateWindCompensationInputs = [this](){
        const bool enabled = ui->checkBox_hasWindCompensation->isChecked();
        windMeasurementChoice->setEnabled(enabled);
        ui->comboBox_windCompensationDistanceEffect->setEnabled(enabled);
        ui->comboBox_windAverageCalculatingType->setEnabled(enabled);
    };
    connect(ui->checkBox_hasWindCompensation, &QCheckBox::toggled, this, updateWindCompensationInputs);
    updateWindCompensationInputs();
    updateResponsiveLayout(width());
    ResponsiveWindowUtils::manageWindowGeometry(this, "SingleJumpsConfigWindow");
}

SingleJumpsConfigWindow::~SingleJumpsConfigWindow()
{
    delete jumperEditor;
    delete hillEditor;
    delete windsGeneratorSettingsEditor;
    delete ui;
}

void SingleJumpsConfigWindow::updateResponsiveLayout(int windowWidth)
{
    const bool useCompactLayout = windowWidth < 900;
    if(useCompactLayout == compactLayout && contentSplitter->orientation() ==
            (compactLayout ? Qt::Vertical : Qt::Horizontal))
        return;

    compactLayout = useCompactLayout;
    contentSplitter->setOrientation(compactLayout ? Qt::Vertical : Qt::Horizontal);
    if(compactLayout)
        contentSplitter->setSizes({qMax(260, height() * 3 / 5), 220});
    else
        contentSplitter->setSizes({qMax(480, windowWidth * 3 / 5), 320});
}

void SingleJumpsConfigWindow::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    updateResponsiveLayout(event->size().width());
}

void SingleJumpsConfigWindow::loadCatalog()
{
    const auto *db=GlobalDatabase::get();const auto packs=db->getPacks();
    const QStringList active{db->activePackId(DataPack::Jumpers),db->activePackId(DataPack::Hills),db->activePackId(DataPack::Rules)};
    const auto maximum=GlobalSimulationSettings::get()->getMaxSkills();const auto service=catalog;
    auto *watcher=new QFutureWatcher<SimulationV2::CatalogSnapshot>(this);
    connect(watcher,&QFutureWatcher<SimulationV2::CatalogSnapshot>::finished,this,[this,watcher]{
        snapshot=watcher->result();watcher->deleteLater();filling=true;
        for(const auto &pack:snapshot.packs) {
            auto *combo=pack.data.kind==DataPack::Jumpers?athletePack:pack.data.kind==DataPack::Hills?hillPack:nullptr;
            if(combo)combo->addItem(pack.data.name,pack.data.id);
        }
        athletePack->setCurrentIndex(athletePack->findData(snapshot.active.value(0)));
        hillPack->setCurrentIndex(hillPack->findData(snapshot.active.value(1)));
        filling=false;ready=true;fillPack(DataPack::Jumpers);fillPack(DataPack::Hills);updateReadiness();
        const int gate=suggestedGate();if(gate>=0)ui->spinBox_gate->setValue(gate);
    });
    watcher->setFuture(QtConcurrent::run([=]{return service.load(packs,maximum,active);}));
}

void SingleJumpsConfigWindow::fillPack(DataPack::Kind kind)
{
    auto *selector=kind==DataPack::Jumpers?athletePack:hillPack;
    const auto *pack=snapshot.find(kind,selector->currentData().toString());
    auto *items=kind==DataPack::Jumpers?ui->comboBox_existingJumper:ui->comboBox_existingHill;
    filling=true;items->clear();
    if(pack) {
        if(kind==DataPack::Jumpers)for(const auto &item:pack->data.jumpers)items->addItem(QIcon(CountryFlagsManager::getFlagPixmap(item.getCountryCode().toLower())),item.getNameAndSurname());
        else for(const auto &item:pack->data.hills)items->addItem(QIcon(CountryFlagsManager::getFlagPixmap(item.getCountryCode().toLower())),item.getName()+" HS"+QString::number(item.getHSPoint()));
        QString error;
        if(snapshot.selectionWritable && !catalog.select(snapshot,kind,pack->data.id,error))snapshot.issues.append(error);
    }
    filling=false;
    if(kind==DataPack::Jumpers)on_comboBox_existingJumper_currentIndexChanged(items->currentIndex());
    else on_comboBox_existingHill_currentIndexChanged(items->currentIndex());
}

void SingleJumpsConfigWindow::updateReadiness()
{
    ui->pushButton_submit->setEnabled(ready && athleteDraft->valid() && hillDraft->valid());
    if(!ready)return;
    QStringList messages;
    if(!athleteDraft->valid())messages.append(tr("Dodaj zawodnika w bazie danych lub wybierz inną paczkę."));
    if(!hillDraft->valid())messages.append(tr("Dodaj skocznię w bazie danych lub wybierz inną paczkę."));
    for(auto kind:{DataPack::Jumpers,DataPack::Hills}) {
        const auto *combo=kind==DataPack::Jumpers?athletePack:hillPack;
        if(const auto *pack=snapshot.find(kind,combo->currentData().toString());pack && !pack->migrationNotice.isEmpty())messages.append(pack->migrationNotice);
    }
    messages.removeDuplicates();
    if(!snapshot.issues.isEmpty())messages.append(tr("Uwagi przy wczytywaniu paczek: %1. Szczegóły są dostępne poniżej.").arg(snapshot.issues.size()));
    findChild<QPushButton *>("pushButton_catalogDetails")->setVisible(!snapshot.issues.isEmpty());
    catalogStatus->setText(messages.join("\n"));catalogStatus->setVisible(!messages.isEmpty());
}

void SingleJumpsConfigWindow::on_comboBox_existingJumper_currentIndexChanged(int index)
{
    if(filling || !ready)return;
    if(index<0 && ui->comboBox_existingJumper->count()>0){ui->comboBox_existingJumper->setCurrentIndex(0);return;}
    const auto *pack=snapshot.find(DataPack::Jumpers,athletePack->currentData().toString());
    if(pack && index>=0 && index<pack->data.jumpers.size()) {
        widgetAthlete=pack->data.jumpers[index];athleteDraft->load(widgetAthlete,pack->directory);
        jumperEditor->setJumper(&widgetAthlete);jumperEditor->fillJumperInputs();
    } else athleteDraft->clear();
}

void SingleJumpsConfigWindow::on_comboBox_existingHill_currentIndexChanged(int index)
{
    if(filling || !ready)return;
    if(index<0 && ui->comboBox_existingHill->count()>0){ui->comboBox_existingHill->setCurrentIndex(0);return;}
    const auto *pack=snapshot.find(DataPack::Hills,hillPack->currentData().toString());
    if(pack && index>=0 && index<pack->data.hills.size()) {
        widgetHill=pack->data.hills[index];hillDraft->load(widgetHill);
        hillEditor->setHill(&widgetHill);hillEditor->fillHillInputs();
    } else hillDraft->clear();
}

void SingleJumpsConfigWindow::on_pushButton_submit_clicked()
{
    if(!ready || !athleteDraft->valid() || !hillDraft->valid())return;
    if(configuredJumper().getNameAndSurname().trimmed().isEmpty()) {
        QMessageBox::warning(this,tr("Brak zawodnika"),tr("Wpisz imię lub nazwisko zawodnika."),QMessageBox::Ok);return;
    }
    accept();
}

SimulationV2::EventKind SingleJumpsConfigWindow::simulationEvent() const
{
    return static_cast<SimulationV2::EventKind>(eventKind->currentIndex());
}

WindsGeneratorSettingsEditorWidget *SingleJumpsConfigWindow::getWindsGeneratorSettingsEditor() const
{
    return windsGeneratorSettingsEditor;
}

int SingleJumpsConfigWindow::getJumpsCountFromInput()
{
    return ui->spinBox_jumpsCount->value();
}

bool SingleJumpsConfigWindow::getChangeableWindFromInput()
{
    return ui->checkBox_changeableWind->isChecked();
}

short SingleJumpsConfigWindow::getWindAverageCalculatingType()
{
    return ui->comboBox_windAverageCalculatingType->currentIndex();
}

int SingleJumpsConfigWindow::getWindMeasurementType() const {return windMeasurementChoice->currentData().toInt();}

short SingleJumpsConfigWindow::getWindCompensationDistanceEffect()
{
    return ui->comboBox_windCompensationDistanceEffect->currentIndex();
}

int SingleJumpsConfigWindow::suggestedGate()
{
    if(!ready || !athleteDraft->valid() || !hillDraft->valid())return -1;
    Jumper jumper = configuredJumper();
    Hill hill = configuredHill();
    if(jumper.getNameAndSurname().trimmed().isEmpty() ||
       !std::isfinite(hill.getHSPoint()) || hill.getHSPoint() <= 0 ||
       hill.getKPoint() <= 0 || hill.getHSPoint() < hill.getKPoint() ||
       !std::isfinite(hill.getPointsForGate()) || hill.getPointsForGate() <= 0 ||
       !std::isfinite(hill.getPointsForMeter()) || hill.getPointsForMeter() <= 0)
        return -1;
    CompetitionRules rules;
    JumpManipulator manipulation;
    JumpSimulator simulator;
    simulator.setJumper(&jumper);simulator.setHill(&hill);
    simulator.setCompetitionRules(&rules);simulator.setManipulator(&manipulation);
    simulator.setSimulationEvent(simulationEvent());
    QVector<Wind> winds;
    const auto settings = windsGeneratorSettingsEditor->getWindsGenerationSettingsFromInputs();
    for(const auto &setting : settings)
        winds.append(Wind(setting.getBaseDirection(), setting.getBaseWindStrength()));
    simulator.setWinds(winds);
    simulator.setSimulationWeather(weatherSetup->field(),std::make_shared<SimulationV2::FieldWindMeasurement>(hill.getKPoint(),rules));
    int low = ui->spinBox_gate->minimum(), high = ui->spinBox_gate->maximum();
    while(low < high) {
        const int middle = (low + high + 1) / 2;
        simulator.setGate(middle);simulator.setCompetitionStartGate(middle);
        const double distance = simulator.previewDistance();
        if(!std::isfinite(distance))return -1;
        if(distance <= hill.getHSPoint() * .97)low = middle;
        else high = middle - 1;
    }
    return low;
}

void SingleJumpsConfigWindow::updateGateSuggestion()
{
    const int suggested=suggestedGate();auto *label=findChild<QLabel *>("label_suggestedGate");
    setProperty("suggestedGate",suggested);
    if(suggested<0){label->clear();return;}
    const int difference=suggested-ui->spinBox_gate->value();
    label->setText(tr("Sugerowana belka: %1").arg(suggested)+(difference?QString(" (%1%2)").arg(difference>0?"+":"").arg(difference):QString()));
}

int SingleJumpsConfigWindow::getGateFromInput()
{
    return ui->spinBox_gate->value();
}

int SingleJumpsConfigWindow::getDSQProbabilityFromInput()
{
    return ui->spinBox_dsqProbability->value();
}

bool SingleJumpsConfigWindow::getHasJudgesFromInput()
{
    return ui->checkBox_hasJudges->isChecked();
}

bool SingleJumpsConfigWindow::getHasWindCompensationFromInput()
{
    return ui->checkBox_hasWindCompensation->isChecked();
}

double SingleJumpsConfigWindow::importance()
{
    return ui->doubleSpinBox_jumpsImportance->value();
}

HillEditorWidget *SingleJumpsConfigWindow::getHillEditor() const
{
    return hillEditor;
}

JumperEditorWidget *SingleJumpsConfigWindow::getJumperEditor() const
{
    return jumperEditor;
}
