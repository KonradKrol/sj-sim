#include "SimulationProfileEditor.h"
#include "../../simulation-v2/ProfileDraft.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSignalBlocker>

QWidget *createSimulationProfileEditor(SimulationProfileDraft *draft,bool hill,QWidget *parent)
{
        auto *panel=new QWidget(parent);auto *form=new QFormLayout(panel);
        const auto addText=[&](const QString &key,const QString &label) {
            auto *input=new QLineEdit(panel);form->addRow(label,input);
            QObject::connect(input,&QLineEdit::editingFinished,draft,[=]{draft->setValue(key,input->text());});
            QObject::connect(draft,&SimulationProfileDraft::changed,input,[=]{const QSignalBlocker guard(input);input->setText(draft->profile().value(key).toString());input->setEnabled(draft->valid());});
        };
        const auto addNumber=[&](const QString &key,const QString &label,double minimum,double maximum,int decimals) {
            auto *input=new QDoubleSpinBox(panel);input->setRange(minimum,maximum);input->setDecimals(decimals);input->setKeyboardTracking(false);form->addRow(label,input);
            QObject::connect(input,QOverload<double>::of(&QDoubleSpinBox::valueChanged),draft,[=](double value){draft->setValue(key,decimals?QVariant(value):QVariant(int(value)));});
            QObject::connect(draft,&SimulationProfileDraft::changed,input,[=]{const QSignalBlocker guard(input);input->setValue(draft->profile().value(key).toDouble());input->setEnabled(draft->valid());});
        };
        addText("name",hill?QObject::tr("Nazwa"):QObject::tr("Imię"));if(!hill)addText("surname",QObject::tr("Nazwisko"));addText("country",QObject::tr("Kraj · kod FIS"));
        if(hill) {
            addNumber("k",QObject::tr("Punkt K"),1,400,1);addNumber("hs","HS",1,500,1);addNumber("record",QObject::tr("Rekord"),0,1000,1);
            addNumber("pointsAtK",QObject::tr("Punkty za K"),0,200,1);addNumber("pointsPerMetre",QObject::tr("Punkty za metr"),0,20,1);
            addNumber("gatePoints",QObject::tr("Punkty za belkę"),0,100,1);addNumber("headwindPoints",QObject::tr("Wiatr przedni · punkty"),0,100,1);addNumber("tailwindPoints",QObject::tr("Wiatr tylny · punkty"),0,100,1);
        } else {
            addNumber("overall","OVR",1,100,0);addNumber("form",QObject::tr("Forma"),1,100,0);addNumber("personalBest",QObject::tr("Rekord życiowy"),0,1000,1);addNumber("birthYear",QObject::tr("Rok urodzenia · 0 = brak"),0,9999,0);addText("photoPath",QObject::tr("Zdjęcie · plik w paczce"));
            auto *landing=new QComboBox(panel);landing->addItems({QObject::tr("Niechlujne"),QObject::tr("Niestaranne"),QObject::tr("Poprawne"),QObject::tr("Eleganckie"),QObject::tr("Stylista")});form->addRow(QObject::tr("Lądowanie"),landing);
            QObject::connect(landing,QOverload<int>::of(&QComboBox::activated),draft,[=](int index){draft->setValue("landing",index);});
            QObject::connect(draft,&SimulationProfileDraft::changed,landing,[=]{const QSignalBlocker guard(landing);landing->setCurrentIndex(draft->profile().value("landing",2).toInt());landing->setEnabled(draft->valid());});
        }
        const QStringList names=hill?QStringList{QObject::tr("Wybicie"),QObject::tr("Lot"),QObject::tr("Łagodniejsze lądowanie"),QObject::tr("Trudniejsze lądowanie"),QObject::tr("Loteryjny wiatr"),QObject::tr("Stabilny wiatr"),QObject::tr("Wiatr przedni"),QObject::tr("Wiatr tylny")}:QStringList{QObject::tr("Błyszczy na treningach"),QObject::tr("Oszczędza siły"),QObject::tr("Lotnik"),QObject::tr("Specjalista małych skoczni"),QObject::tr("Regularny"),QObject::tr("Nieprzewidywalny")};
        for(int pair=0;pair<names.size()/2;++pair) {
            auto *perk=new QComboBox(panel);perk->addItem(QObject::tr("Brak"),0);
            for(int side=0;side<2;++side)for(int tier=1;tier<=3;++tier)perk->addItem(names[pair*2+side]+QString(" · %1").arg(tier),side?-tier:tier);
            form->addRow(QObject::tr("Perk %1").arg(pair+1),perk);
            QObject::connect(perk,QOverload<int>::of(&QComboBox::activated),draft,[=](int index){const int value=perk->itemData(index).toInt();const int old=draft->selectedPerks().value(pair).toInt();if(value)draft->setPerk(pair,value>0?1:-1,qAbs(value),false);else if(old)draft->setPerk(pair,old>0?1:-1,qAbs(old),true);});
            QObject::connect(draft,&SimulationProfileDraft::changed,perk,[=]{const QSignalBlocker guard(perk);perk->setCurrentIndex(perk->findData(draft->selectedPerks().value(pair).toInt()));perk->setEnabled(draft->valid());});
        }
        auto *status=new QLabel(panel);status->setWordWrap(true);form->addRow(status);
        QObject::connect(draft,&SimulationProfileDraft::changed,status,[=]{status->setText(draft->message());});
        return panel;
}
