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
    minForm = newMinForm;
}

double JumperFormGeneratorSettings::getMaxForm() const
{
    return maxForm;
}

void JumperFormGeneratorSettings::setMaxForm(double newMaxForm)
{
    maxForm = newMaxForm;
}
