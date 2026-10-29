#include "UI/mainwindow.h"
#include "playground/PlaygroundExperiment.h"
#include <cstring>
#ifdef SJ_QUICK_UI
#include "UI/Quick/QuickUi.h"
#endif

#include <QApplication>
#include <QDir>
#include <QIcon>
#include <QTranslator>
#include <QTimer>
#include "global/GlobalTranslators.h"
#include "global/GlobalAppSettings.h"

int main(int argc, char *argv[])
{
    for(int i=1;i<argc;++i)if(std::strcmp(argv[i],"--playground")==0 || std::strcmp(argv[i],"--playground-example")==0 || std::strcmp(argv[i],"--balance-lab")==0 || std::strcmp(argv[i],"--balance-lab-example")==0)return Playground::cli(argc,argv);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication a(argc, argv);
    QApplication::setOrganizationName("SJ-Sim");
    QApplication::setApplicationName("SJ-Sim");
    QApplication::setWindowIcon(QIcon(":/sj-sim.png"));

    // Data files are deliberately shipped beside the executable. Do not rely
    // on the shell/shortcut choosing that directory as its working directory.
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    GlobalAppSettings::get()->loadFromJson();

    if (!GlobalTranslators::get()->setLanguage(&a, GlobalAppSettings::get()->getLanguageID())) {
        GlobalAppSettings::get()->setLanguageID(GlobalAppSettings::Polish);
        GlobalTranslators::get()->setLanguage(&a, GlobalAppSettings::Polish);
    }

    MainWindow w;
    w.setParentApplication(&a);
#ifdef SJ_QUICK_UI
    if (QuickUi::enabled()) new QuickUi(&w);
#endif
    w.show();
    if (a.arguments().contains("--quick-competition") || a.arguments().contains("--widgets-competition"))
        QTimer::singleShot(0, &w, [&w] {
            QMetaObject::invokeMethod(&w, "on_pushButton_singleCompetition_clicked");
        });
    return a.exec();
}
