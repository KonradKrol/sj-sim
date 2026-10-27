#pragma once
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSignalBlocker>
#include <QVector>
#include "../../global/JumpsImportancePreset.h"

namespace ConfigSelections {
inline void importancePresets(QComboBox *choice,QDoubleSpinBox *value,const QVector<JumpsImportancePreset> &presets) {
    {
        const QSignalBlocker blocker(choice);
        choice->clear();
        for(const auto &preset:presets)
            choice->addItem(preset.getName()+" ("+QString::number(preset.getJumpsImportance(),'f',2)+")",preset.getJumpsImportance());
        choice->addItem(QObject::tr("Własne ustawienia"));
        choice->setCurrentIndex(0);
    }
    if(choice->currentData().isValid())value->setValue(choice->currentData().toDouble());
    QObject::connect(choice,qOverload<int>(&QComboBox::currentIndexChanged),value,[choice,value](int index){
        if(index<0 && choice->count()>0){choice->setCurrentIndex(0);return;}
        if(choice->currentData().isValid())value->setValue(choice->currentData().toDouble());
    });
    QObject::connect(value,qOverload<double>(&QDoubleSpinBox::valueChanged),choice,[choice](double number){
        int selected=choice->count()-1;
        for(int i=0;i<choice->count()-1;++i)
            if(qAbs(choice->itemData(i).toDouble()-number)<0.000001){selected=i;break;}
        const QSignalBlocker blocker(choice);choice->setCurrentIndex(selected);
    });
}
}
