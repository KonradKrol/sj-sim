#ifndef JUMPERFORMGENERATOR_H
#define JUMPERFORMGENERATOR_H

#include "JumperFormGeneratorSettings.h"

class JumperFormGenerator
{
public:
    JumperFormGenerator();

public:
    void generateJumperForm();
private:
    double getBaseRandom();
private:
    JumperFormGeneratorSettings settings;

    Jumper * jumper;
    double * instability;
public:
    JumperFormGeneratorSettings & getSettingsReference();
    void setSettings(const JumperFormGeneratorSettings &newSettings);
    Jumper *getJumper() const;
    double *getInstability() const;
    void setInstability(double *newInstability);
};

#endif // JUMPERFORMGENERATOR_H
