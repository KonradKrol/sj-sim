#include <QApplication>
#include <QTemporaryDir>
#include <QSettings>
#include <QQuickWidget>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickItem>
#include <QTimer>
#include <QEventLoop>
#include <QDebug>
#include <cstdlib>
#include "../source/UI/Quick/QuickUi.h"
#include "../source/UI/Quick/QuickSurface.h"
static void check(bool condition,const char *message){if(!condition){qCritical()<<"FAIL:"<<message;std::exit(1);}}
static void tick(){QEventLoop loop;QTimer::singleShot(150,&loop,&QEventLoop::quit);loop.exec();}
int main(int argc,char **argv){QApplication app(argc,argv);QTemporaryDir temp;QSettings::setDefaultFormat(QSettings::IniFormat);QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,temp.path());
    qmlRegisterType<QuickSurface>("SjSim",1,0,"SculptedSurface");QuickAppearance appearance;QQuickWidget view;view.setResizeMode(QQuickWidget::SizeRootObjectToView);auto *engine=view.engine();
    view.rootContext()->setContextProperty("appearance",&appearance);view.rootContext()->setContextProperty("navigation",QVariant::fromValue<QObject*>(nullptr));
    QQmlComponent mock(engine);mock.setData("import QtQuick 2.15\nQtObject {property var context: ({showWelcome:true,exampleBusy:false});property string last: '';function launch(s){last=s}}",QUrl());auto *ui=mock.create();check(ui,"mock backend");view.rootContext()->setContextProperty("ui",ui);
    QQmlComponent page(engine);page.setData("import QtQuick 2.15\nimport \"qrc:/ui\"\nRectangle {color: Theme.background;HomeWelcome{}}",QUrl("qrc:/welcome-layout-check.qml"));if(page.isError()){qCritical()<<page.errors();return 1;}
    auto *root=page.create();check(root,"welcome component");view.setContent(QUrl(),&page,root);view.resize(1000,700);view.show();tick();auto *popup=root->findChild<QObject*>("homeWelcome");check(popup,"welcome opens");
    auto *next=popup->findChild<QQuickItem*>("homeWelcomeNext");auto *secondary=popup->findChild<QQuickItem*>("homeWelcomeSecondary");auto *frame=popup->findChild<QQuickItem*>("welcomeImageFrame");auto *image=popup->findChild<QQuickItem*>("welcomeImage");check(next&&secondary&&frame&&image,"expected welcome controls");
    for(int step=0;step<4;++step){popup->setProperty("step",step);tick();check(next->x()>secondary->x(),"primary action stays on right");check(frame->height()<popup->property("height").toDouble()*.65,"image frame remains compact");check(image->property("fillMode").toInt()==2,"image fills and crops its frame");view.grabFramebuffer().save(QString("/tmp/sjsim-welcome-%1.png").arg(step));}
    check(next->property("text").toString()=="Ok","last primary action is Ok");check(secondary->property("text").toString()=="Rozpocznij przykładowy sezon","last secondary action offers example");
    QMetaObject::invokeMethod(secondary,"clicked");check(ui->property("last").toString()=="example","secondary starts example");QMetaObject::invokeMethod(next,"clicked");check(ui->property("last").toString()=="dismissWelcome","Ok goes to menu");
    view.resize(640,480);tick();view.grabFramebuffer().save("/tmp/sjsim-welcome-small.png");check(frame->height()>0&&next->isVisible(),"small window keeps image and actions");
    qInfo()<<"PASS welcome layout, four steps, crop sizing, right-aligned actions and final navigation";
}
