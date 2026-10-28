#include "JumpManipulatorConfigWindow.h"
#include "ui_JumpManipulatorConfigWindow.h"
#include "../ResponsiveWindowUtils.h"

#include "../EditorWidgets/WindsGeneratorSettingsEditorWidget.h"

#include <QPair>
#include <functional>
#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QTabWidget>
#ifdef SJ_QUICK_UI
#include "../Quick/QuickUi.h"
#endif
#include "../../simulation-v2/JumpDraft.h"

namespace {
// Widgets remains an optional presentation of the same validated draft.
QWidget *nativeJumpEditor(SimulationJumpDraft *draft,JumpManipulatorConfigWindow *dialog)
{
    auto *host=new QWidget(dialog);host->setObjectName("nativeJumpDraftEditor");auto *layout=new QVBoxLayout(host);
    auto *tabs=new QTabWidget(host);layout->addWidget(tabs,1);
    const auto page=[&](const QString &title) {
        auto *scroll=new QScrollArea(tabs);scroll->setWidgetResizable(true);auto *body=new QWidget(scroll);auto *column=new QVBoxLayout(body);column->setAlignment(Qt::AlignTop);scroll->setWidget(body);tabs->addTab(scroll,title);return column;
    };
    auto *flight=page(QObject::tr("Lot")),*landing=page(QObject::tr("Lądowanie i noty")),*conditions=page(QObject::tr("Warunki"));
    const auto check=[&](QVBoxLayout *column,const QString &label,const QString &key) {
        auto *control=new QCheckBox(label,host);column->addWidget(control);
        QObject::connect(control,&QCheckBox::toggled,draft,[draft,key](bool value){draft->setValue(key,value);});
        const auto refresh=[draft,control,key]{const QSignalBlocker block(control);control->setChecked(draft->values()[key].toBool());};
        QObject::connect(draft,&SimulationJumpDraft::changed,control,refresh);refresh();return control;
    };
    const auto number=[&](QFormLayout *form,const QString &label,const QString &key,double low,double high,const QString &enabledKey=QString()) {
        auto *control=new QDoubleSpinBox(host);control->setObjectName("nativeAdjustment-"+key);control->setRange(low,high);control->setDecimals(2);control->setKeyboardTracking(false);control->setSingleStep(.5);form->addRow(label,control);
        QObject::connect(control,QOverload<double>::of(&QDoubleSpinBox::valueChanged),draft,[draft,key](double value){draft->setValue(key,value);});
        const auto refresh=[draft,control,key,enabledKey]{const QSignalBlocker block(control);control->setValue(draft->values()[key].toDouble());control->setEnabled(enabledKey.isEmpty() || draft->values()[enabledKey].toBool());};
        QObject::connect(draft,&SimulationJumpDraft::changed,control,refresh);refresh();return control;
    };
    const auto adjustment=[&](QVBoxLayout *column,const QString &label,const QString &key,double low,double high) {
        auto *box=new QGroupBox(host);auto *content=new QVBoxLayout(box);column->addWidget(box);
        check(content,label,key+"Active");auto *form=new QFormLayout;content->addLayout(form);
        number(form,QObject::tr("Korekta"),key+"Bonus",-high,high,key+"Active");
        check(content,QObject::tr("Ogranicz zakres"),key+"Bounded");auto *range=new QWidget(box);auto *bounds=new QFormLayout(range);content->addWidget(range);
        number(bounds,QObject::tr("Minimum"),key+"Min",low,high);number(bounds,QObject::tr("Maksimum"),key+"Max",low,high);
        const auto refresh=[draft,key,range]{range->setEnabled(draft->values()[key+"Active"].toBool() && draft->values()[key+"Bounded"].toBool());};
        QObject::connect(draft,&SimulationJumpDraft::changed,range,refresh);refresh();
    };
    adjustment(flight,QObject::tr("Jakość skoku"),"quality",0,1000);adjustment(flight,QObject::tr("Odległość"),"distance",0,1000);
    auto *landingChoice=new QComboBox(host);landingChoice->addItems({QObject::tr("Losuj"),QObject::tr("Telemark"),QObject::tr("Na dwie nogi"),QObject::tr("Podparty"),QObject::tr("Upadek")});
    auto *landingForm=new QFormLayout;landing->addLayout(landingForm);landingForm->addRow(QObject::tr("Lądowanie"),landingChoice);
    QObject::connect(landingChoice,QOverload<int>::of(&QComboBox::activated),draft,[draft](int index){draft->setValue("landing",index-1);});
    const auto refreshLanding=[draft,landingChoice]{const QSignalBlocker block(landingChoice);landingChoice->setCurrentIndex(draft->values()["landing"].toInt()+1);};QObject::connect(draft,&SimulationJumpDraft::changed,landingChoice,refreshLanding);refreshLanding();
    for(int i=0;i<5;++i) {
        auto *row=new QHBoxLayout;landing->addLayout(row);auto *enabled=new QCheckBox(QObject::tr("Nota sędziego %1").arg(i+1),host);auto *value=new QDoubleSpinBox(host);value->setRange(0,20);value->setSingleStep(.5);value->setDecimals(1);value->setKeyboardTracking(false);row->addWidget(enabled);row->addWidget(value);
        QObject::connect(enabled,&QCheckBox::toggled,draft,[draft,i](bool active){draft->setJudge(i,active?18:-1);});QObject::connect(value,QOverload<double>::of(&QDoubleSpinBox::valueChanged),draft,[draft,i](double v){draft->setJudge(i,v);});
        const auto refresh=[draft,i,enabled,value]{const QSignalBlocker a(enabled),b(value);const double v=draft->values()["judges"].toList()[i].toDouble();enabled->setChecked(v>=0);value->setEnabled(v>=0);value->setValue(qMax(0.,v));};QObject::connect(draft,&SimulationJumpDraft::changed,value,refresh);refresh();
    }
    if(draft->values()["dsqAllowed"].toBool()) {
        auto *risk=new QFormLayout;landing->addLayout(risk);auto *value=number(risk,QObject::tr("Ryzyko dyskwalifikacji · %"),"dsq",-1,100);value->setSpecialValueText(QObject::tr("Losuj"));
    }
    check(conditions,QObject::tr("Ustaw wiatr dla tego skoku"),"windActive");
    auto *weather=qobject_cast<SimulationWeatherSetup *>(draft->wind());auto *stations=new QWidget(host);auto *grid=new QGridLayout(stations);conditions->addWidget(stations);
    grid->addWidget(new QLabel(QObject::tr("Odcinek"),host),0,0);grid->addWidget(new QLabel(QObject::tr("Kierunek · °"),host),0,1);grid->addWidget(new QLabel("m/s",host),0,2);
    for(int i=0;i<6;++i) {
        grid->addWidget(new QLabel(QString::number(i+1),host),i+1,0);
        for(int j=0;j<2;++j) {
            const QString key=j?"strength":"direction";auto *value=new QDoubleSpinBox(host);value->setRange(0,j?15:360);value->setDecimals(j?1:0);value->setKeyboardTracking(false);grid->addWidget(value,i+1,j+1);
            QObject::connect(value,QOverload<double>::of(&QDoubleSpinBox::valueChanged),weather,[weather,i,key](double v){weather->setStation(i,key,v);});
            const auto refresh=[weather,i,key,value]{const QSignalBlocker block(value);value->setValue(weather->station(i)[key].toDouble());};QObject::connect(draft,&SimulationJumpDraft::changed,value,refresh);refresh();
        }
    }
    const auto refreshStations=[draft,stations]{stations->setEnabled(draft->values()["windActive"].toBool());};QObject::connect(draft,&SimulationJumpDraft::changed,stations,refreshStations);refreshStations();
    adjustment(conditions,QObject::tr("Natężenie opadów"),"precipitation",0,10);adjustment(conditions,QObject::tr("Pomiar do punktacji"),"measuredWind",-100,100);
    auto *error=new QLabel(host);error->setWordWrap(true);layout->addWidget(error);auto *actions=new QHBoxLayout;layout->addLayout(actions);auto *reset=new QPushButton(QObject::tr("Wyczyść"),host);auto *apply=new QPushButton(QObject::tr("Zastosuj"),host);apply->setObjectName("nativeJumpApply");actions->addWidget(reset);actions->addStretch();actions->addWidget(apply);
    QObject::connect(reset,&QPushButton::clicked,draft,&SimulationJumpDraft::reset);QObject::connect(apply,&QPushButton::clicked,dialog,[dialog,draft]{if(draft->canApply())dialog->submitted();});
    const auto refreshError=[draft,error,apply]{error->setText(draft->message());apply->setEnabled(draft->canApply());};QObject::connect(draft,&SimulationJumpDraft::changed,error,refreshError);refreshError();
    return host;
}
}

JumpManipulatorConfigWindow::JumpManipulatorConfigWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JumpManipulatorConfigWindow)
{
    ui->setupUi(this);
}

JumpManipulatorConfigWindow::~JumpManipulatorConfigWindow()
{
    delete ui;
}

JumpManipulator JumpManipulatorConfigWindow::getJumpManipulatorFromInputs() const
{
    if(draft)return draft->manipulator();
    JumpManipulator m;
    m.setDistanceRange(QPair<double, double>(ui->doubleSpinBox_minDistance->value(), ui->doubleSpinBox_maxDistance->value()));
    m.setInrunSnowRange(QPair<double, double>(ui->doubleSpinBox_minInrunSnow->value(), ui->doubleSpinBox_maxInrunSnow->value()));
    m.setTakeoffRatingRange(QPair<double, double>(ui->doubleSpinBox_minTakeoffRating->value(), ui->doubleSpinBox_maxTakeoffRating->value()));
    m.setFlightRatingRange(QPair<double, double>(ui->doubleSpinBox_minFlightRating->value(), ui->doubleSpinBox_maxFlightRating->value()));
    if(ui->checkBox_disableAveragedWindRange->isChecked() == false){
        m.setAveragedWindRange(QPair<double, double>(ui->doubleSpinBox_minAveragedWind->value(), ui->doubleSpinBox_maxAveragedWind->value()));
        m.setAveragedWindRangeEnabled(true);
    }
    else m.setAveragedWindRangeEnabled(false);

    m.setDistanceBonus(ui->doubleSpinBox_distanceBonus->value());
    m.setInrunSnowBonus(ui->doubleSpinBox_inrunSnowBonus->value());
    m.setTakeoffRatingBonus(ui->doubleSpinBox_takeoffRatingBonus->value());
    m.setFlightRatingBonus(ui->doubleSpinBox_flightRatingBonus->value());
    m.setAveragedWindBonus(ui->doubleSpinBox_averagedWindBonus->value());

    m.setExactLandingType(ui->comboBox_landingType->currentIndex() - 1);
    QVector<double> judges;
    judges.resize(5);
    judges[0] = ui->doubleSpinBox_judge1->value();
    judges[1] = ui->doubleSpinBox_judge2->value();
    judges[2] = ui->doubleSpinBox_judge3->value();
    judges[3] = ui->doubleSpinBox_judge4->value();
    judges[4] = ui->doubleSpinBox_judge5->value();
    m.setExactJudges(judges);

    m.setExactDSQProbability(ui->spinBox_dsqProbability->value());
    m.setExactWinds(this->exactWinds);

    return m;
}

void JumpManipulatorConfigWindow::on_checkBox_disableAveragedWindRange_stateChanged(int arg1)
{
    if(arg1 == 2){
        ui->doubleSpinBox_minAveragedWind->setEnabled(false);
        ui->doubleSpinBox_maxAveragedWind->setEnabled(false);
        ui->doubleSpinBox_minAveragedWind->setStyleSheet(ui->doubleSpinBox_minAveragedWind->styleSheet());
        ui->doubleSpinBox_maxAveragedWind->setStyleSheet(ui->doubleSpinBox_maxAveragedWind->styleSheet());
    }
    else{
        ui->doubleSpinBox_minAveragedWind->setEnabled(true);
        ui->doubleSpinBox_maxAveragedWind->setEnabled(true);
        ui->doubleSpinBox_minAveragedWind->setStyleSheet(ui->doubleSpinBox_minAveragedWind->styleSheet());
        ui->doubleSpinBox_maxAveragedWind->setStyleSheet(ui->doubleSpinBox_maxAveragedWind->styleSheet());
    }
}


void JumpManipulatorConfigWindow::on_pushButton_submit_2_clicked()
{
    if(draft && !draft->canApply())return;
    emit submitted();
}

void JumpManipulatorConfigWindow::configureSimulationV2(const JumpManipulator &value,const HillProfile &hill,const QVector<Wind> &winds,bool allowDsq)
{
    if(!draft) {
        draft=new SimulationJumpDraft(this);draft->setObjectName("jumpAdjustmentDraft");
        setProperty("jumpDraft",QVariant::fromValue<QObject *>(draft));
        connect(draft,&SimulationJumpDraft::changed,this,[this]{ui->pushButton_submit_2->setEnabled(draft->canApply());});
    }
    setWindowTitle(tr("Zmiana następnego skoku"));setMinimumSize(320,240);
    ui->pushButton_submit_2->setText(tr("Zastosuj"));
    draft->load(value,hill,winds,allowDsq);
#ifdef SJ_QUICK_UI
    if(QuickUi::enabled())return;
#endif
    if(!findChild<QWidget *>("nativeJumpDraftEditor")) {
        auto *legacy=new QWidget(this);auto *hiddenLayout=new QVBoxLayout(legacy);
        const std::function<void(QLayoutItem *)> reparent=[&](QLayoutItem *entry) {
            if(auto *widget=entry->widget())widget->setParent(legacy);
            else if(auto *nested=entry->layout())for(int i=0;i<nested->count();++i)reparent(nested->itemAt(i));
        };
        while(auto *entry=ui->verticalLayout_4->takeAt(0)){reparent(entry);hiddenLayout->addItem(entry);}
        legacy->hide();ui->verticalLayout_4->addWidget(nativeJumpEditor(draft,this));
    }
}


void JumpManipulatorConfigWindow::on_pushButton_editWinds_clicked()
{
    QVector<WindGenerationSettings> fallback(WindsGenerator::calculateWindsCountByKPoint(KPoint));
    QDialog dialog(this);
    dialog.setModal(true);
    dialog.setWindowFlags(Qt::Window);
    dialog.setWindowTitle(tr("Edytuj wiatr"));
    dialog.setStyleSheet("background-color: rgb(225, 225, 225);");
    dialog.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dialog.setLayout(new QVBoxLayout(&dialog));

    WindsGeneratorSettingsEditorWidget * windsEditor = new WindsGeneratorSettingsEditorWidget(&dialog);
    windsEditor->setKPoint(this->getKPoint());
    windsEditor->setWindGenerationSettings(windGenerationSettings?windGenerationSettings:&fallback);
    windsEditor->fillSettingsInputsToExactWindsEditor();
    ResponsiveWindowUtils::addScrollableContent(&dialog, windsEditor);
    dialog.raise();

    connect(windsEditor, &WindsGeneratorSettingsEditorWidget::submitted, &dialog, &QDialog::accept);
    ResponsiveWindowUtils::fitToAvailableScreen(&dialog, QSize(560, 640));
    if(dialog.exec() == QDialog::Accepted){
        this->exactWinds = windsEditor->getExactWindsFromInputs();
    }
}

double JumpManipulatorConfigWindow::getKPoint() const
{
    return KPoint;
}

void JumpManipulatorConfigWindow::setKPoint(double newKPoint)
{
    KPoint = newKPoint;
}

QVector<WindGenerationSettings> *JumpManipulatorConfigWindow::getWindGenerationSettings() const
{
    return windGenerationSettings;
}

void JumpManipulatorConfigWindow::setWindGenerationSettings(QVector<WindGenerationSettings> *newWindGenerationSettings)
{
    windGenerationSettings = newWindGenerationSettings;
}
