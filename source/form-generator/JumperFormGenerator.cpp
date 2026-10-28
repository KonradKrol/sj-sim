#include "JumperFormGenerator.h"
#include <QDebug>
#include "../global/MyRandom.h"
#include <algorithm>
#include <cmath>

JumperFormGenerator::JumperFormGenerator()
{

}

void JumperFormGenerator::generateJumperForm()
{
    if(!jumper || !instability)return;
    auto profile=jumper->getSimulationProfile();
    double oldForm = profile?profile->form:jumper->getJumperSkillsPointer()->getForm();
    double newForm;
    double oldInstability = *instability;
    double newInstability = oldInstability;
    double change = 0;

    if(oldForm > 50){
        if(MyRandom::randomDouble(0, 2, 5) <= 1 - oldInstability)
            change = MyRandom::normalDistributionRandomHalf(0, 11, 0);
        else
            change = MyRandom::normalDistributionRandomHalf(0, 11, 1);
    }
    else{
        if(MyRandom::randomDouble(0, 2, 5) <= 1 - oldInstability)
           change = MyRandom::normalDistributionRandomHalf(0, 11, 1);
        else
           change = MyRandom::normalDistributionRandomHalf(0, 11, 0);
    }
    change *= settings.getFormVariability();
    if(oldForm > 50 && change > 0)
    {
        change /= 1 + ((oldForm - 50) / 11.3);
    }
    else if(oldForm > 50 && change < 0)
    {
        change /= 1 + ((oldForm - 50) / 41);
    }
    else if(oldForm < 50 && change < 0)
    {
        change /= 1 + ((50 - (oldForm)) / 11.3);
    }
    else if(oldForm < 50 && change > 0)
    {
        change /= 1 + ((50 - (oldForm)) / 41);
    }

    if(settings.getFormVariability()>0)newInstability /= (1.30 * settings.getFormVariability());
    const double denominator=std::max(1.0,oldForm+change);
    const double previousDenominator=std::max(1.0,oldForm);
    if(oldForm + change > 50 && change > 0)
    {
        newInstability += (((oldForm + change) / previousDenominator) - 1) / 1.5;
    }
    else if(oldForm + change < 50 && change < 0)
    {
        newInstability += ((oldForm / denominator) - 1) / 1.5;
    }
    else if(oldForm + change > 50 && change < 0)
    {
        newInstability += (((oldForm + change) / previousDenominator) - 1) / 1.5;
    }
    else if(oldForm + change < 50 && change > 0)
    {
        newInstability += ((oldForm / denominator) - 1) / 1.5;
    }
    if(newInstability < 0.01)
        newInstability = 0.01;
    if(newInstability > 0.99)
        newInstability = 0.99;

    if(settings.getFormVariability() == 0)
    {
        newInstability = oldInstability;
        change = 0;
        if(oldForm == 50)
           newInstability = 0;
    }

    newForm = oldForm + change + settings.getFormBonus();
    if(newForm < settings.getMinForm())
        newForm = settings.getMinForm();
    else if(newForm > settings.getMaxForm())
        newForm = settings.getMaxForm();

    if(profile) {
        profile->form=int(std::round(std::clamp(newForm,1.0,100.0)));
        QString error;if(!jumper->setSimulationProfile(*profile,error))return;
    } else jumper->getJumperSkillsPointer()->setForm(newForm);
    *instability = newInstability;
    qDebug()<<jumper->getNameAndSurname() + QString(": (Forma %1 --> %2) (Niestabilność %3 --> %4)").arg(QString::number(oldForm)).arg(QString::number(newForm)).arg(QString::number(oldInstability)).arg(QString::number(newInstability));
}

double JumperFormGenerator::getBaseRandom()
{
    //int random = MyRandom::normalDistributionRandomHalf(0, 100, 1);

    return 0;
}

double *JumperFormGenerator::getInstability() const
{
    return instability;
}

void JumperFormGenerator::setInstability(double *newInstability)
{
    instability = newInstability;
}

Jumper *JumperFormGenerator::getJumper() const
{
    return jumper;
}

JumperFormGeneratorSettings &JumperFormGenerator::getSettingsReference()
{
    return settings;
}

void JumperFormGenerator::setSettings(const JumperFormGeneratorSettings &newSettings)
{
    settings = newSettings;
    jumper = settings.getJumper();
}
