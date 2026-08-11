#include "UI/mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QTranslator>
#include "global/GlobalTranslators.h"
#include "global/GlobalAppSettings.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication a(argc, argv);
    QApplication::setOrganizationName("SJ-Sim");
    QApplication::setApplicationName("SJ-Sim");

    // Data files are deliberately shipped beside the executable. Do not rely
    // on the shell/shortcut choosing that directory as its working directory.
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    GlobalAppSettings::get()->loadFromJson();

    if(GlobalAppSettings::get()->getLanguageID() == 1) //english
    a.installTranslator(GlobalTranslators::get()->getGlobalTranslations().at(0));

    MainWindow w;
    w.setParentApplication(&a);
    w.show();
    return a.exec();
}
