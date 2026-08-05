#include "JumperFormGenerator.h"
#include "../global/MyRandom.h"

JumperFormGenerator::JumperFormGenerator()
{

}

void JumperFormGenerator::generateJumperForm()
{
    double oldForm = jumper->getJumperSkillsPointer()->getForm();
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

    newInstability /= (1.30 * settings.getFormVariability());
    if(oldForm + change > 50 && change > 0)
    {
        newInstability += (((oldForm + change) / oldForm) - 1) / 1.5;
    }
    else if(oldForm + change < 50 && change < 0)
    {
        newInstability += ((oldForm / (oldForm + change)) - 1) / 1.5;
    }
    else if(oldForm + change > 50 && change < 0)
    {
        newInstability += (((oldForm + change) / oldForm) - 1) / 1.5;
    }
    else if(oldForm + change < 50 && change > 0)
    {
        newInstability += ((oldForm / (oldForm + change)) - 1) / 1.5;
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

    jumper->getJumperSkillsPointer()->setForm(newForm);
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
