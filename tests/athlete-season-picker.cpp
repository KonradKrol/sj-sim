#include <QApplication>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlContext>
#include <QDebug>
#include <cstdlib>
#include <QMouseEvent>
#include <qqml.h>
#include "../source/UI/Quick/QuickSurface.h"
static QQuickItem *visual(QQuickItem *item,const QString &name){if(item->objectName()==name)return item;for(auto *child:item->childItems())if(auto *found=visual(child,name))return found;return nullptr;}
static void check(bool ok,const char *why){if(!ok){qCritical()<<why;std::exit(1);}}
int main(int argc,char **argv){
 QApplication app(argc,argv);qmlRegisterType<QuickSurface>("SjSim",1,0,"SculptedSurface");QQuickView view;
 view.rootContext()->setContextProperty("appearance",QVariant::fromValue<QObject *>(nullptr));
 view.rootContext()->setContextProperty("navigation",QVariant::fromValue<QObject *>(nullptr));
 view.setSource(QUrl("qrc:/ui/AthleteResultsTable.qml"));check(view.status()==QQuickView::Ready,"statistics table loads");
 auto *table=view.rootObject();
 const auto season=[](const char *key,const char *label,int count){return QVariantMap{{"key",key},{"label",label},{"season",label},{"competitions",QVariantList{}},{"count",count},{"totalText",""}};};
 const auto first=season("2027-winter","2027/2028",1),second=season("2026-winter","2026/2027",2);
 table->setProperty("tables",QVariantList{first,second});app.processEvents();
 auto *picker=table->findChild<QObject *>("athleteStatisticsSeason");check(picker,"season selector exists");
 view.setResizeMode(QQuickView::SizeRootObjectToView);view.resize(700,300);view.show();app.processEvents();
 auto *pickerItem=qobject_cast<QQuickItem *>(picker);const auto point=pickerItem->mapToScene(QPointF(pickerItem->width()/2,pickerItem->height()/2));
 QMouseEvent press(QEvent::MouseButtonPress,point,Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);QApplication::sendEvent(&view,&press);
 QMouseEvent release(QEvent::MouseButtonRelease,point,Qt::LeftButton,Qt::NoButton,Qt::NoModifier);QApplication::sendEvent(&view,&release);app.processEvents();
 auto *popup=picker->property("popup").value<QObject *>();check(popup && popup->property("visible").toBool() && popup->property("height").toDouble()>0,"actual mouse click opens visible season options");
 auto *option=visual(view.contentItem(),"choiceOption-1");check(option,"previous season option exists");
 const auto optionPoint=option->mapToScene(QPointF(option->width()/2,option->height()/2));
 QMouseEvent optionPress(QEvent::MouseButtonPress,optionPoint,Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);QApplication::sendEvent(&view,&optionPress);
 QMouseEvent optionRelease(QEvent::MouseButtonRelease,optionPoint,Qt::LeftButton,Qt::NoButton,Qt::NoModifier);QApplication::sendEvent(&view,&optionRelease);app.processEvents();
 check(table->property("selectedKey").toString()=="2026-winter","mouse selection actually changes the season");
 QMetaObject::invokeMethod(popup,"close");app.processEvents();

 picker->setProperty("currentIndex",1);check(QMetaObject::invokeMethod(picker,"activated",Q_ARG(int,1)),"select previous season");
 check(table->property("selectedKey").toString()=="2026-winter","selected season changes");
 table->setProperty("tables",QVariantList{first,season("2026-winter","2026/2027",3)});app.processEvents();
 check(picker->property("currentIndex").toInt()==1 && table->property("tables").toList().size()==2,"data refresh preserves selection and updates results");
 table->setProperty("tables",QVariantList{second,first});app.processEvents();
 check(table->property("selectedKey").toString()=="2026-winter" && picker->property("currentIndex").toInt()==0,"reordering follows the selected season identity");
 qInfo()<<"PASS: statistics season selection survives data refresh and reordering";
}
