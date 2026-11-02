#pragma once
#include "../../competitions/CompetitionRules.h"
#include <QComboBox>
#include <QWidget>

inline QComboBox *createWindMeasurementChoice(QWidget *parent)
{
    auto *choice=new QComboBox(parent);choice->setObjectName("comboBox_windMeasurementType");
    const auto add=[&](const QString &name,int mode,const QString &help) {
        choice->addItem(name,mode);choice->setItemData(choice->count()-1,help,Qt::ToolTipRole);
    };
    add(QObject::tr("FIS — przybliżenie"),CompetitionRules::FisApproximationWindMeasurement,
        QObject::tr("Przybliżony pomiar FIS: czujniki mają różne wagi, także za miejscem lądowania. Średnia obejmuje czas lotu."));
    add(QObject::tr("Pomiar przez czas lotu"),CompetitionRules::SensorWindMeasurement,
        QObject::tr("Wszystkie czujniki mają równą wagę, także za miejscem lądowania. Średnia obejmuje czas lotu."));
    add(QObject::tr("Gęsta sieć czujników"),CompetitionRules::DenseWindMeasurement,
        QObject::tr("Gęstsza sieć czujników, także za miejscem lądowania. Średnia obejmuje czas lotu."));
    add(QObject::tr("Odczyt przed skokiem"),CompetitionRules::InstantWindMeasurement,
        QObject::tr("Jednorazowy odczyt wszystkich czujników przy wybiciu. Późniejsze zmiany wiatru nie są uwzględniane."));
    add(QObject::tr("Wiatr na trasie lotu"),CompetitionRules::FlightWindMeasurement,
        QObject::tr("Średnia wiatru napotkanego przez skoczka podczas lotu, tylko na przebytej trasie."));
    add(QObject::tr("Dotychczasowy Sj.Sim"),CompetitionRules::LegacyWindMeasurement,
        QObject::tr("Dawny pomiar Sj.Sima. Kierunki i wpływ odległości ustawisz osobno."));
    const auto update=[choice]{choice->setToolTip(choice->currentData(Qt::ToolTipRole).toString());};
    QObject::connect(choice,QOverload<int>::of(&QComboBox::currentIndexChanged),choice,[update]{update();});update();
    choice->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    return choice;
}

inline void bindLegacyWindInputs(QComboBox *choice,const QList<QWidget *> &fields)
{
    const auto update=[choice,fields]{for(auto *field:fields)field->setVisible(choice->currentData().toInt()==CompetitionRules::LegacyWindMeasurement);};
    QObject::connect(choice,QOverload<int>::of(&QComboBox::currentIndexChanged),choice,[update]{update();});update();
}
