#include "UI/mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include "global/GlobalTranslators.h"
#include "global/GlobalAppSettings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GlobalAppSettings::get()->loadFromJson();

    if(GlobalAppSettings::get()->getLanguageID() == 1) //english
    a.installTranslator(GlobalTranslators::get()->getGlobalTranslations().at(0));

    MainWindow w;
    w.setParentApplication(&a);
    w.show();
    return a.exec();
}
