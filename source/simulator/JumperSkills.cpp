#include "JumperSkills.h"
#include "Jumper.h"

JumperSkills::JumperSkills(double takeoffPower, double takeoffTechnique, double flightTechnique, short flightStyle, double form, double landingStyle, const QSet<Characteristic> &characteristics, Jumper * jumper) :
    takeoffTechnique(takeoffTechnique),
    flightStyle(flightStyle),
    flightTechnique(flightTechnique),
    form(form),
    landingStyle(landingStyle),
    Identifiable()
{
    if(jumper != nullptr)
        jumperID = jumper->getID();

    setCharacteristics(characteristics);
    jumpsEquality = 0;
}

void JumperSkills::checkSkillsLimits()
{
    if(getFlightStyle() < 0)
        setFlightStyle(0);
    else if(getFlightStyle() > 3)
        setFlightStyle(3);
}

Jumper *JumperSkills::getJumper() const
{
    return jumper;
}

void JumperSkills::setJumper(Jumper *newJumper)
{
    jumper = newJumper;
}

double JumperSkills::getForm() const
{
    return form;
}

void JumperSkills::setForm(double newForm)
{
    form = newForm;
}

void JumperSkills::setJumperID(sole::uuid ID)
{
    jumperID = ID;
}

double JumperSkills::getTakeoffTechnique() const
{
    return takeoffTechnique;
}

void JumperSkills::setTakeoffTechnique(double newTakeoffTechnique)
{
    takeoffTechnique = newTakeoffTechnique;
}

short JumperSkills::getFlightStyle() const
{
    return flightStyle;
}

void JumperSkills::setFlightStyle(short newFlightStyle)
{
    flightStyle = newFlightStyle;
}

double JumperSkills::getFlightTechnique() const
{
    return flightTechnique;
}

void JumperSkills::setFlightTechnique(double newFlightTechnique)
{
    flightTechnique = newFlightTechnique;
}

double JumperSkills::getLandingStyle() const
{
    return landingStyle;
}

void JumperSkills::setLandingStyle(double newLandingStyle)
{
    landingStyle = newLandingStyle;
}

QSet<Characteristic> JumperSkills::getCharacteristics() const
{
    return characteristics;
}

void JumperSkills::setCharacteristics(const QSet<Characteristic> &newCharacteristics)
{
    characteristics = newCharacteristics;
}

double JumperSkills::getJumpsEquality() const
{
    return jumpsEquality;
}

void JumperSkills::setJumpsEquality(double newJumpsEquality)
{
    jumpsEquality = newJumpsEquality;
}
