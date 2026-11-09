#pragma once
#include "../../competitions/CompetitionRules.h"
#include <QComboBox>
#include <QWidget>

inline QComboBox *createWindMeasurementChoice(QWidget *parent)
{
    auto *choice=new QComboBox(parent);choice->setObjectName("comboBox_windMeasurementType");choice->setAccessibleName(QObject::tr("Pomiar wiatru"));
    const auto add=[&](const QString &name,int mode,const QString &help) {
        choice->addItem(name,mode);choice->setItemData(choice->count()-1,help,Qt::ToolTipRole);
    };
    add(QObject::tr("Domyślny"),CompetitionRules::FisApproximationWindMeasurement,
        QObject::tr("Ważona średnia z czujników przez czas lotu, również zza miejsca lądowania. Pomiar może różnić się od warunków na trasie skoczka."));
    add(QObject::tr("Ulepszony"),CompetitionRules::ImprovedWindMeasurement,
        QObject::tr("Jak Domyślny, ale pomija czujniki za miejscem lądowania. Nadal uśrednia pomiary przez cały czas lotu."));
    add(QObject::tr("Idealny"),CompetitionRules::FlightWindMeasurement,
        QObject::tr("Dokładna średnia wiatru na rzeczywistej trasie lotu. Stosuje zwykłe przeliczniki punktowe, więc nie gwarantuje pełnego wyrównania wpływu wiatru na wynik."));
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
