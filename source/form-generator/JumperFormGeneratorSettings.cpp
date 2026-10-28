#include "JumperFormGeneratorSettings.h"

JumperFormGeneratorSettings::JumperFormGeneratorSettings()
{
    reset();
}

void JumperFormGeneratorSettings::reset()
{
    formVariability = 1;
    formBonus = minForm = 0;
    maxForm = 100;
    jumper = nullptr;
}

QVector<JumperFormGeneratorSettings> JumperFormGeneratorSettings::constructJumperFormGeneratorsSettingsVector(QVector<Jumper *> &jumpers)
{
    QVector<JumperFormGeneratorSettings> toReturn;
    for(auto & jumper : jumpers)
    {
        JumperFormGeneratorSettings settings;
        settings.setJumper(jumper);
        toReturn.push_back(settings);
    }
    return toReturn;
}

Jumper *JumperFormGeneratorSettings::getJumper() const
{
    return jumper;
}

void JumperFormGeneratorSettings::setJumper(Jumper *newJumper)
{
    jumper = newJumper;
    if(jumper && jumper->getSimulationProfile()) {
        minForm=qBound(1.0,minForm,100.0);maxForm=qBound(minForm,maxForm,100.0);
    }
}

double JumperFormGeneratorSettings::getFormVariability() const
{
    return formVariability;
}

void JumperFormGeneratorSettings::setFormVariability(double newFormVariability)
{
    formVariability = newFormVariability;
}

double JumperFormGeneratorSettings::getFormBonus() const
{
    return formBonus;
}

void JumperFormGeneratorSettings::setFormBonus(double newFormBonus)
{
    formBonus = newFormBonus;
}

double JumperFormGeneratorSettings::getMinForm() const
{
    return minForm;
}

void JumperFormGeneratorSettings::setMinForm(double newMinForm)
{
    minForm = jumper && jumper->getSimulationProfile()?qBound(1.0,newMinForm,100.0):newMinForm;
    if(jumper && jumper->getSimulationProfile() && maxForm<minForm)maxForm=minForm;
}

double JumperFormGeneratorSettings::getMaxForm() const
{
    return maxForm;
}

void JumperFormGeneratorSettings::setMaxForm(double newMaxForm)
{
    maxForm = jumper && jumper->getSimulationProfile()?qBound(minForm,newMaxForm,100.0):newMaxForm;
}
