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
        QObject::tr("Ważony pomiar z nieruchomych czujników przez czas lotu. Wagi i rozmieszczenie są przybliżone; nie jest to dokładna oficjalna receptura FIS. Pomiar może rozmijać się z warunkami zawodnika."));
    add(QObject::tr("Pomiar przez czas lotu"),CompetitionRules::SensorWindMeasurement,
        QObject::tr("Równe wagi czujników przez cały lot. Krótki podmuch może umknąć pomiarowi lub wpływać na wynik już po minięciu czujnika."));
    add(QObject::tr("Gęsta sieć czujników"),CompetitionRules::DenseWindMeasurement,
        QObject::tr("Pomiar co około 5 m przez czas lotu. Lepiej wykrywa lokalne podmuchy, ale nadal nie odtwarza warunków zawodnika co do chwili."));
    add(QObject::tr("Odczyt przed skokiem"),CompetitionRules::InstantWindMeasurement,
        QObject::tr("Jednorazowy odczyt czujników przy wybiciu. Zmiany w trakcie lotu nie wpływają na pomiar do punktacji."));
    add(QObject::tr("Wiatr na trasie lotu"),CompetitionRules::FlightWindMeasurement,
        QObject::tr("Średnia wiatru rzeczywiście napotkanego podczas lotu. Punkt odniesienia do symulacji, bez idealnego wyrównania wpływu na odległość."));
    add(QObject::tr("Dotychczasowy Sj.Sim"),CompetitionRules::LegacyWindMeasurement,
        QObject::tr("Zachowuje wcześniejsze ustawienia sposobu kierunków i wpływu odległości na pomiar. Nie oznacza zgodności z FIS."));
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
