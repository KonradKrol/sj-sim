#include <QApplication>
#include <QEventLoop>
#include <QTimer>
#include <QQuickWidget>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQmlContext>
#include <QDebug>
#include <QMouseEvent>
#include <qqml.h>
#include "../source/UI/Quick/QuickSurface.h"
#include "../source/simulation-v2/ProfileDraft.h"
static void check(bool ok,const char *message){if(!ok){qCritical()<<message;std::exit(1);}}
static QQuickItem *visual(QQuickItem *item,const QString &name){if(item->objectName()==name)return item;for(auto *child:item->childItems())if(auto *found=visual(child,name))return found;return nullptr;}
int main(int argc,char **argv){
 QApplication app(argc,argv);qmlRegisterType<QuickSurface>("SjSim",1,0,"SculptedSurface");
 Jumper athlete;auto profile=AthleteProfile::create();profile.name="Test";profile.surname="Athlete";QString error;check(athlete.setSimulationProfile(profile,error),"athlete");
 SimulationProfileDraft backend(DataPack::Jumpers);backend.load(athlete,QString());backend.setFormDiagnostic({{"automatic",true},{"pending",false},{"regimeIndex",3},{"gradualSteeringLocked",false}});
 QQuickWidget view;view.rootContext()->setContextProperty("appearance",QVariant::fromValue<QObject *>(nullptr));view.rootContext()->setContextProperty("navigation",QVariant::fromValue<QObject *>(nullptr));
 view.setSource(QUrl("qrc:/ui/FormShortcuts.qml"));check(view.status()==QQuickWidget::Ready,"form actions load");auto *root=view.rootObject();root->setProperty("backend",QVariant::fromValue<QObject *>(&backend));view.setResizeMode(QQuickWidget::SizeRootObjectToView);view.resize(660,430);view.show();app.processEvents();
 auto *trigger=visual(root,"athleteFormShortcuts");check(trigger,"trigger exists");QMetaObject::invokeMethod(trigger,"clicked");app.processEvents();
 auto *popup=root->findChild<QObject *>("athleteFormShortcutsMenu");check(popup && popup->property("visible").toBool(),"menu opens");
 for(int action=0;action<=6;++action)if(action!=4){auto *button=visual(view.quickWindow()->contentItem(),QString("athleteFormAction-%1").arg(action));check(button && button->isEnabled(),"all six actions available");}
 backend.setFormDiagnostic({{"automatic",true},{"pending",false},{"regimeIndex",4},{"gradualSteeringLocked",true}});app.processEvents();check(!visual(view.quickWindow()->contentItem(),"athleteFormAction-0")->isEnabled() && !visual(view.quickWindow()->contentItem(),"athleteFormAction-1")->isEnabled(),"both gradual actions disabled together");
 QEventLoop wait;QTimer::singleShot(300,&wait,&QEventLoop::quit);wait.exec();
 if(qEnvironmentVariableIsSet("SJ_FORM_SCREENSHOT"))view.grabFramebuffer().save("/tmp/sjsim-form-actions.png");
 qInfo()<<"PASS: compact form actions load, open, and expose gradual lock";
}
