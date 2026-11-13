#include <QApplication>
#include <QDebug>
#include <QTemporaryDir>
#include <QSettings>
#include <cmath>
#include <cstdlib>
static void check(bool value,const QString &message){if(!value){qCritical().noquote()<<"FAIL:"<<message;std::exit(1);}}
#include "../source/simulation-v2/WeatherSetup.h"
int main(int argc,char **argv){
    QApplication app(argc,argv);QTemporaryDir temporary;
    QSettings::setDefaultFormat(QSettings::IniFormat);QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,temporary.path());
    SimulationWeatherSetup setup;auto hill=HillProfile::create();setup.setHill(hill);
    setup.setLevel("variability",2);const auto seed=setup.values()["seed"];
    setup.setCustom("variability");check(setup.values()["variabilityCustom"].toBool() && setup.values()["seed"]==seed,"custom mode keeps the existing weather");
    setup.setValue("variability",37);check(setup.values()["variability"].toDouble()==37,"exact temporal value accepted");
    setup.setValue("differences",73);check(setup.values()["differencesCustom"].toBool() && setup.values()["variability"].toDouble()==37,"spatial input does not change temporal input");
    setup.setValue("strength",2.4);check(setup.values()["differencesCustom"].toBool() && setup.values()["variabilityCustom"].toBool(),"strength preserves custom selectors");
    setup.reroll();check(setup.values()["variability"].toDouble()==37 && setup.values()["differences"].toDouble()==73,"reroll preserves exact requested settings");
    setup.setLevel("variability",2);check(!setup.values()["variabilityCustom"].toBool() && setup.values()["variability"].toDouble()>=40 && setup.values()["variability"].toDouble()<59,"named level restores its range");
    setup.applyPreset(0);check(!setup.values()["variabilityCustom"].toBool() && !setup.values()["differencesCustom"].toBool(),"preset resets both selectors");
    qInfo()<<"PASS independent weather controls, custom levels, presets and rerolling";
}
