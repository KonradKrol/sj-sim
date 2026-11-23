#include "../source/global/SettingsMigrations.h"
#include <QCoreApplication>
#include <QTemporaryDir>
#include <iostream>
int main(int argc,char **argv){
 QCoreApplication app(argc,argv);QTemporaryDir directory;
 QSettings settings(directory.filePath("settings.ini"),QSettings::IniFormat);
 settings.setValue("simulation/simple/distanceSpread",4);settings.setValue("simulation/simple/spread",0);
 SettingsMigrations::apply210(settings);
 if(settings.value("simulation/simple/distanceSpread").toInt()!=2 || settings.value("simulation/simple/spread").toInt()!=0)return 1;
 settings.setValue("simulation/simple/distanceSpread",1);settings.sync();
 QSettings restarted(directory.filePath("settings.ini"),QSettings::IniFormat);SettingsMigrations::apply210(restarted);
 if(restarted.value("simulation/simple/distanceSpread").toInt()!=1)return 2;
 std::cout<<"PASS: reset once, preserve subsequent choice and other settings\n";
}
