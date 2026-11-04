#include "UI/mainwindow.h"
#include "playground/PlaygroundExperiment.h"
#include <cstring>
#ifdef SJ_QUICK_UI
#include "UI/Quick/QuickUi.h"
#include <QQuickWidget>
#endif

#include <QApplication>
#include <QDir>
#include <QIcon>
#include <QTranslator>
#include <QTimer>
#include <QDebug>
#include <QLockFile>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFile>
#include <cstdio>
#include <cstdlib>
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
    const bool deploymentSmoke=a.arguments().contains("--deployment-smoke");
    const auto deploymentTrace=[deploymentSmoke](const char *stage) {
        if(!deploymentSmoke)return;
        QFile log(QCoreApplication::applicationDirPath()+"/deployment-smoke.log");
        if(log.open(QIODevice::WriteOnly|QIODevice::Append)){log.write(stage);log.write("\n");}
    };
    deploymentTrace("QApplication ready");
    if(deploymentSmoke) {
        QStandardPaths::setTestModeEnabled(true);
        QApplication::setOrganizationName("SJ-Sim-Deploy-Test");
        QTimer *dialogs=new QTimer(&a);
        QObject::connect(dialogs,&QTimer::timeout,[]{
            for(auto *widget:QApplication::allWidgets())if(auto *message=qobject_cast<QMessageBox *>(widget);message && (message->isVisible() || message->property("sjQuickPage").toBool())) {
                std::fprintf(stderr,"Deployment startup dialog: %s: %s\n",qPrintable(message->windowTitle()),qPrintable(message->text()));
                std::fflush(stderr);std::exit(3);
            }
        });
        dialogs->start(100);
    }
    QApplication::setWindowIcon(QIcon(":/sj-sim.png"));

    // Data files are deliberately shipped beside the executable. Do not rely
    // on the shell/shortcut choosing that directory as its working directory.
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    // Portable files and shared authoring profiles have separate roots.
    // Keep both locks alive until every window and writer has shut down.
    QDir().mkpath("userData");
    QLockFile dataLock(QDir("userData").absoluteFilePath(".sj-sim.lock"));
    const auto authoringRoot=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(authoringRoot);
    QLockFile authoringLock(authoringRoot+"/.sj-sim.lock");
    if(!dataLock.tryLock(0) || !authoringLock.tryLock(0)) {
        QMessageBox::warning(nullptr, QObject::tr("Sj.Sim jest już uruchomiony"),
            QObject::tr("Dane gry lub paczki edytorów są używane przez inną instancję Sj.Sim albo nie można ich zapisać.\nZamknij poprzednie okno Sj.Sim i spróbuj ponownie."));
        return 1;
    }
    deploymentTrace("Data lock acquired");

    if(!GlobalAppSettings::get()->loadFromJson())
        qWarning() << "Application settings could not be loaded; keeping safe defaults.";
    deploymentTrace("Settings loaded");

    if (!GlobalTranslators::get()->setLanguage(&a, GlobalAppSettings::get()->getLanguageID())) {
        GlobalAppSettings::get()->setLanguageID(GlobalAppSettings::Polish);
        GlobalTranslators::get()->setLanguage(&a, GlobalAppSettings::Polish);
    }

    deploymentTrace("Creating main window");
    MainWindow w;
    deploymentTrace("Main window ready");
    w.setParentApplication(&a);
#ifdef SJ_QUICK_UI
    if (QuickUi::enabled()) new QuickUi(&w);
    deploymentTrace("Quick UI created");
#endif
    w.show();
    if(a.arguments().contains("--deployment-smoke")) {
        QTimer::singleShot(1500,&a,[&] {
#ifdef SJ_QUICK_UI
            const auto views=w.findChildren<QQuickWidget *>();
            bool ready=!views.isEmpty();
            for(auto *view:views)ready=ready && view->status()==QQuickWidget::Ready && view->rootObject();
            qInfo()<<"Deployment UI check:"<<(ready?"PASS":"FAIL");
            deploymentTrace(ready?"PASS: Quick UI ready":"FAIL: Quick UI unavailable");
            a.exit(ready?0:2);
#else
            a.exit(2);
#endif
        });
    }
    if (a.arguments().contains("--quick-competition") || a.arguments().contains("--widgets-competition"))
        QTimer::singleShot(0, &w, [&w] {
            QMetaObject::invokeMethod(&w, "on_pushButton_singleCompetition_clicked");
        });
    return a.exec();
}
