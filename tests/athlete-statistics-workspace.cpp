#include "../source/UI/Quick/QuickUi.h"
#include "../source/UI/Seasons/Stats/JumperStatsWindow.h"
#include "../source/UI/mainwindow.h"
#include "../source/global/UserDataPaths.h"
#include "../source/seasons/SimulationSave.h"

#include <QApplication>
#include <QDebug>
#include <QEventLoop>
#include <QMouseEvent>
#include <QPointer>
#include <QQuickItem>
#include <QQuickWidget>
#include <QQuickWindow>
#include <QSettings>
#include <QTemporaryDir>
#include <QTimer>
#include <cstdlib>

static void check(bool ok,const char *message)
{
    if(!ok){qCritical()<<message;std::exit(1);}
}

static void wait(int milliseconds)
{
    QEventLoop loop;
    QTimer::singleShot(milliseconds,&loop,&QEventLoop::quit);
    loop.exec();
}

static QQuickItem *findItem(QQuickItem *parent,const QString &name)
{
    if(parent->objectName()==name)return parent;
    for(auto *child:parent->childItems())
        if(auto *match=findItem(child,name))return match;
    return nullptr;
}

static void click(QQuickWidget *view,QQuickItem *item)
{
    const auto point=item->mapToScene(item->boundingRect().center());
    check(view->rect().contains(point.toPoint()),"clicked control fits in the window");
    QMouseEvent press(QEvent::MouseButtonPress,point,Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
    QMouseEvent release(QEvent::MouseButtonRelease,point,Qt::LeftButton,Qt::NoButton,Qt::NoModifier);
    QApplication::sendEvent(view,&press);
    QApplication::sendEvent(view,&release);
}

int main(int argc,char **argv)
{
    QApplication app(argc,argv);
    app.setOrganizationName("SJ-Sim-Athlete-Statistics-Test");
    app.setApplicationName("Isolated");
    QTemporaryDir directory;
    check(directory.isValid(),"temporary library");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,directory.path()+"/settings");
    app.setProperty("sjSimDataRoot",directory.path());
    app.setProperty("sjSimDefaultsRoot",QString::fromLocal8Bit(argv[1])+"/data/defaults");
    QString error;
    check(UserDataPaths::initialize(error),qPrintable(error));

    MainWindow main;
    main.setParentApplication(&app);
    new QuickUi(&main);
    main.resize(820,530);
    main.show();

    SimulationSave save;
    auto *athlete=new Jumper("A","Jumper","POL");
    auto *hill=new Hill;
    save.setJumpers({athlete});
    save.setHills({hill});
    QVector<Season> seasons;
    for(int year : {2026,2027}){
        auto *calendar=new SeasonCalendar("Zima");
        auto *event=new CompetitionInfo(hill);
        CompetitionRules rules;
        rules.setRounds({RoundInfo(1,true)});
        event->setRules(rules);
        event->setPlayed(true);
        JumpData jump(athlete,hill);
        jump.setCompetition(event);
        jump.setDistance(100);
        jump.setPoints(100);
        event->getResultsReference().addJump(athlete,jump);
        event->getResultsReference().sortInDescendingOrder();
        calendar->setCompetitions({event});
        Season season;
        season.setSeasonNumber(year);
        season.setCalendars({calendar});
        season.setActualCalendar(calendar);
        seasons.append(season);
    }
    save.setSeasons(seasons);
    save.setActualSeason(&save.getSeasonsReference().last());
    JumperStatsWindow stats(&main);
    stats.setSave(&save);
    stats.setJumper(athlete);
    stats.setupComboBox();
    stats.show();
    wait(250);

    auto *bridge=stats.findChild<QuickBridge *>();
    check(bridge && bridge->view() && bridge->view()->rootObject(),"full statistics workspace loads");
    auto *view=bridge->view();
    QPointer<QQuickItem> picker=findItem(view->rootObject(),"athleteStatisticsSeason");
    check(picker && picker->isVisible() && picker->property("count").toInt()==2,"both season choices exist");
    click(view,picker);
    wait(450);
    check(picker && picker==findItem(view->rootObject(),"athleteStatisticsSeason"),"periodic refresh keeps the selector alive");
    auto *popup=picker->property("popup").value<QObject *>();
    check(popup && popup->property("visible").toBool(),"the popup stays open across refreshes");

    auto *option=findItem(view->quickWindow()->contentItem(),"choiceOption-1");
    check(option && option->isVisible(),"previous season is visible");
    click(view,option);
    wait(450);
    check(picker && picker->property("currentIndex").toInt()==1,"the selected season survives periodic refresh");
    check(!popup->property("visible").toBool(),"the popup closes after selecting a season");
    qInfo()<<"PASS: statistics season dropdown opens and selects through real mouse input during refresh";
}
