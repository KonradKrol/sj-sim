#include "WeatherSetupEditor.h"
#include "../../simulation-v2/WeatherSetup.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QSignalBlocker>
#include <QGroupBox>
#include <QLabel>
#include <QStandardItemModel>

QWidget *createWeatherSetupEditor(SimulationWeatherSetup *backend,QWidget *parent) {
    auto *panel=new QWidget(parent);auto *form=new QFormLayout(panel);
    auto *presets=new QComboBox(panel);presets->addItem(QObject::tr("Losowo · według skoczni"));
    for(const auto &preset:backend->presets())presets->addItem(preset.toString());
    presets->addItem(QObject::tr("Własne warunki"));form->addRow(QObject::tr("Wiatr"),presets);
    if(auto *model=qobject_cast<QStandardItemModel *>(presets->model()))model->item(presets->count()-1)->setEnabled(false);
    presets->setCurrentIndex(backend->values()["modified"].toBool()?presets->count()-1:backend->values()["preset"].toInt()+1);
    QObject::connect(presets,QOverload<int>::of(&QComboBox::activated),backend,[=](int index){if(index==0)backend->suggest();else if(index<=backend->presets().size())backend->applyPreset(index-1);});
    QObject::connect(backend,&SimulationWeatherSetup::changed,presets,[=]{const QSignalBlocker block(presets);const auto values=backend->values();presets->setCurrentIndex(values["modified"].toBool()?presets->count()-1:values["preset"].toInt()+1);presets->setEnabled(values["ready"].toBool());});
    auto *reroll=new QPushButton(QObject::tr("Losuj ponownie"),panel);form->addRow(reroll);QObject::connect(reroll,&QPushButton::clicked,backend,&SimulationWeatherSetup::reroll);
    auto *tuning=new QGroupBox(QObject::tr("Dostosuj wiatr"),panel);tuning->setCheckable(true);tuning->setChecked(false);form->addRow(tuning);
    auto *tuningLayout=new QFormLayout(tuning);auto *controls=new QWidget(tuning);auto *numbers=new QFormLayout(controls);numbers->setContentsMargins(0,0,0,0);tuningLayout->addRow(controls);controls->hide();
    QObject::connect(tuning,&QGroupBox::toggled,controls,&QWidget::setVisible);
    const auto number=[&](const QString &key,const QString &label,double maximum,double factor=1) {
        auto *input=new QDoubleSpinBox(panel);input->setRange(0,maximum);input->setDecimals(1);input->setKeyboardTracking(false);numbers->addRow(label,input);
        input->setValue(backend->values().value(key).toDouble()*factor);
        QObject::connect(input,QOverload<double>::of(&QDoubleSpinBox::valueChanged),backend,[=](double value){backend->setValue(key,value/factor);});
        QObject::connect(backend,&SimulationWeatherSetup::changed,input,[=]{const QSignalBlocker block(input);input->setValue(backend->values().value(key).toDouble()*factor);});
    };
    number("direction",QObject::tr("Kierunek · 0° pod narty, 180° w plecy"),360);
    number("strength",QObject::tr("Siła wyjściowa · m/s"),10);
    number("variability",QObject::tr("Zmienność w czasie · 0–10"),10,.1);
    number("differences",QObject::tr("Różnice na zeskoku · 0–10"),10,.1);
    const auto choice=[&](const QString &key,const QString &label,const QStringList &options) {
        auto *input=new QComboBox(panel);input->addItems(options);form->addRow(label,input);
        input->setCurrentIndex(backend->values()[key].toInt());
        if(key=="precipitationPattern") {
            input->setEnabled(backend->values()["precipitationLevel"].toInt()>0);
            QObject::connect(backend,&SimulationWeatherSetup::changed,input,[=]{input->setEnabled(backend->values()["precipitationLevel"].toInt()>0);});
        }
        QObject::connect(input,QOverload<int>::of(&QComboBox::activated),backend,[=](int index){backend->setValue(key,index);});
        QObject::connect(backend,&SimulationWeatherSetup::changed,input,[=]{const QSignalBlocker block(input);input->setCurrentIndex(backend->values()[key].toInt());});
    };
    choice("precipitationKind",QObject::tr("Rodzaj opadów"),{QObject::tr("Śnieg"),QObject::tr("Deszcz")});
    choice("precipitationLevel",QObject::tr("Natężenie"),{QObject::tr("Bez opadów"),QObject::tr("Słabe"),QObject::tr("Umiarkowane"),QObject::tr("Silne")});
    choice("precipitationPattern",QObject::tr("Przebieg"),{QObject::tr("Stałe"),QObject::tr("Zmienne"),QObject::tr("Nasilające się")});
    auto *explanation=new QLabel(panel);explanation->setWordWrap(true);form->addRow(explanation);
    const auto updateExplanation=[=]{explanation->setText(backend->values()["precipitationKind"].toInt()==0?QObject::tr("Śnieg w torach spowalnia rozbieg."):QObject::tr("Lekki deszcz nie musi hamować. Ulewa pogarsza ślizg."));};
    QObject::connect(backend,&SimulationWeatherSetup::changed,explanation,updateExplanation);updateExplanation();
    return panel;
}
