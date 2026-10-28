#include "WeatherSetupEditor.h"
#include "../../simulation-v2/WeatherSetup.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QSignalBlocker>

QWidget *createWeatherSetupEditor(SimulationWeatherSetup *backend,QWidget *parent) {
    auto *panel=new QWidget(parent);auto *form=new QFormLayout(panel);
    auto *presets=new QComboBox(panel);presets->addItems({QObject::tr("Propozycja skoczni"),QObject::tr("Stabilny w plecy"),QObject::tr("Mocno pod narty"),QObject::tr("Loteria"),QObject::tr("Zmienny boczny"),QObject::tr("Cisza"),QObject::tr("Własne")});form->addRow(QObject::tr("Scenariusz"),presets);
    QObject::connect(presets,QOverload<int>::of(&QComboBox::activated),backend,[=](int index){if(index==0)backend->suggest();else if(index<=5)backend->applyPreset(index-1);});
    QObject::connect(backend,&SimulationWeatherSetup::changed,presets,[=]{const QSignalBlocker block(presets);const auto values=backend->values();presets->setCurrentIndex(values["modified"].toBool()?6:values["preset"].toInt()+1);presets->setEnabled(values["ready"].toBool());});
    auto *reroll=new QPushButton(QObject::tr("Losuj ponownie"),panel);form->addRow(reroll);QObject::connect(reroll,&QPushButton::clicked,backend,&SimulationWeatherSetup::reroll);
    const auto number=[&](const QString &key,const QString &label,double maximum,double factor=1) {
        auto *input=new QDoubleSpinBox(panel);input->setRange(0,maximum);input->setDecimals(1);input->setKeyboardTracking(false);form->addRow(label,input);
        QObject::connect(input,QOverload<double>::of(&QDoubleSpinBox::valueChanged),backend,[=](double value){backend->setValue(key,value/factor);});
        QObject::connect(backend,&SimulationWeatherSetup::changed,input,[=]{const QSignalBlocker block(input);input->setValue(backend->values().value(key).toDouble()*factor);});
    };
    number("direction",QObject::tr("Kierunek · 0° pod narty, 180° w plecy"),360);
    number("strength",QObject::tr("Siła wyjściowa · m/s"),10);
    number("variability",QObject::tr("Zmienność w czasie · 0–10"),10,.1);
    number("differences",QObject::tr("Różnice na zeskoku · 0–10"),10,.1);
    auto *surface=new QComboBox(panel);surface->addItems({QObject::tr("Śnieg"),QObject::tr("Deszcz")});form->addRow(QObject::tr("Opady"),surface);
    QObject::connect(surface,QOverload<int>::of(&QComboBox::activated),backend,[=](int index){backend->setValue("surface",index);});
    QObject::connect(backend,&SimulationWeatherSetup::changed,surface,[=]{const QSignalBlocker block(surface);surface->setCurrentIndex(backend->values()["surface"].toInt());});
    auto *precipitation=new QComboBox(panel);precipitation->addItems({QObject::tr("Brak"),QObject::tr("Stałe"),QObject::tr("Zmienne"),QObject::tr("Bardzo silne"),QObject::tr("Nasilające się")});form->addRow(QObject::tr("Przebieg opadów"),precipitation);
    QObject::connect(precipitation,QOverload<int>::of(&QComboBox::activated),backend,[=](int index){backend->setValue("precipitationScenario",index);});
    QObject::connect(backend,&SimulationWeatherSetup::changed,precipitation,[=]{const QSignalBlocker block(precipitation);precipitation->setCurrentIndex(backend->values()["precipitationScenario"].toInt());});
    number("precipitationIntensity",QObject::tr("Natężenie opadów · 0–10"),10);
    return panel;
}
