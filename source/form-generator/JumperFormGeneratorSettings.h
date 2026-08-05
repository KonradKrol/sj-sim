#ifndef JUMPERFORMGENERATORSETTINGS_H
#define JUMPERFORMGENERATORSETTINGS_H

#include <QVector>
#include "../simulator/Jumper.h"

class JumperFormGeneratorSettings
{
public:
    JumperFormGeneratorSettings();
    void reset();

    static QVector<JumperFormGeneratorSettings> constructJumperFormGeneratorsSettingsVector(QVector<Jumper *> & jumpers);

private:
    //FORM
    double formVariability;
    double formBonus;
    double minForm;
    double maxForm;

    Jumper * jumper;
public:
    double getFormVariability() const;
    void setFormVariability(double newFormVariability);
    double getFormBonus() const;
    void setFormBonus(double newFormBonus);
    double getMinForm() const;
    void setMinForm(double newMinForm);
    double getMaxForm() const;
    void setMaxForm(double newMaxForm);
    Jumper *getJumper() const;
    void setJumper(Jumper *newJumper);
};

#endif // JUMPERFORMGENERATORSETTINGS_H
