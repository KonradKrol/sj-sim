#include "QuickUi.h"
#include "QuickRoster.h"
#include "QuickPlot.h"
#include "QuickSurface.h"
#include "../mainwindow.h"
#include <QFontDatabase>
#include <QApplication>
#include <QDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QQuickWidget>
#include <QQuickItem>
#include <QQuickStyle>
#include <QQmlContext>
#include <QQmlEngine>
#include <QScreen>
#include <QShortcut>
#include <QCloseEvent>
#include <QTimer>
#include <QWindow>
#include <QDebug>
#include <QtMath>

namespace {
QQuickWidget *makeView(QWidget *parent,QuickBridge *bridge,QuickAppearance *appearance,QuickUi *navigation,const QString &source,bool page=true) {
    auto *view=new QQuickWidget(parent);
    view->setObjectName(page?"sjQuickView":"sjQuickChrome");
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    view->rootContext()->setContextProperty("ui",bridge);
    view->rootContext()->setContextProperty("appearance",appearance);
    view->rootContext()->setContextProperty("navigation",navigation);
    view->setProperty("quickSource",source);
    if(page)bridge->setView(view);
    view->setSource(QUrl(source));
    if(view->status()==QQuickWidget::Error)for(const auto &error:view->errors())qWarning().noquote()<<error.toString();
    return view;
}
void fitChrome(QQuickWidget *view) {
    if(auto *root=view->rootObject()) {
        const auto resize=[view,root]{view->setFixedHeight(qCeil(root->implicitHeight()));};
        QObject::connect(root,&QQuickItem::implicitHeightChanged,view,resize);resize();
    }
}
bool isTaskController(QWidget *widget) {
    return widget && (qobject_cast<QDialog *>(widget) || widget->inherits("JumpDataDetailedInfoWindow"))
        && !qobject_cast<QFileDialog *>(widget) && !qobject_cast<QMessageBox *>(widget)
        && !qobject_cast<QInputDialog *>(widget) && !qobject_cast<QProgressDialog *>(widget)
        && !widget->property("sjQuickUtility").toBool();
}
void keepControllerOffscreen(QWidget *widget) {
    widget->setAttribute(Qt::WA_DontShowOnScreen);
    widget->setAttribute(Qt::WA_ShowWithoutActivating);
    widget->setWindowModality(Qt::NonModal);
    widget->setAttribute(Qt::WA_ShowModal,false);
}
}
bool QuickUi::enabled() {
    return !qEnvironmentVariableIsSet("SJ_WIDGETS_UI") && !QCoreApplication::arguments().contains("--widgets");
}
QuickUi::QuickUi(MainWindow *window) : QObject(window),m_main(window),m_appearance(new QuickAppearance(qApp)) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // The custom controls replace native backgrounds and content items. Select
    // a customizable style before any QML engine registers the controls.
    if(QQuickStyle::name()!=QStringLiteral("Basic"))QQuickStyle::setStyle(QStringLiteral("Basic"));
#endif
    for(const auto &font:{"Barlow-Regular.ttf","Barlow-Medium.ttf","Barlow-SemiBold.ttf","BarlowCondensed-ExtraBold.ttf","IBMPlexMono-Regular.ttf"})
        if(QFontDatabase::addApplicationFont(QString(":/ui/fonts/")+font)<0)qWarning()<<"Cannot load interface font"<<font;
    QApplication::setFont(QFont("Barlow",12));
    qmlRegisterUncreatableType<QuickRosterRows>("SjSim",1,0,"RosterRows","Roster model");
    qmlRegisterType<QuickSurface>("SjSim",1,0,"SculptedSurface");
    qmlRegisterUncreatableType<QuickNode>("SjSim",1,0,"ControlNode","View adapter");
    qmlRegisterUncreatableType<QuickBridge>("SjSim",1,0,"ScreenBridge","Screen controller");
    qmlRegisterType<QuickPlot>("SjSim",1,0,"SimulationPlot");
    auto *legacy=window->takeCentralWidget();legacy->setParent(window);legacy->hide();legacy->setObjectName("widgetsHomeContent");
    auto *host=new QWidget(window);host->setObjectName("quickApplicationShell");
    auto *layout=new QVBoxLayout(host);layout->setContentsMargins(0,0,0,0);layout->setSpacing(0);
    m_stack=new QStackedWidget(host);m_stack->setObjectName("quickPageStack");
    auto *bridge=new QuickBridge(window,legacy,m_appearance,true);
    auto *view=makeView(m_stack,bridge,m_appearance,this,"qrc:/ui/Home.qml");
    m_stack->addWidget(view);m_pages.append({window,bridge});
    auto *header=makeView(host,bridge,m_appearance,this,"qrc:/ui/AppHeader.qml",false);header->setObjectName("quickApplicationHeader");fitChrome(header);
    auto *footer=makeView(host,bridge,m_appearance,this,"qrc:/ui/AppFooter.qml",false);footer->setObjectName("quickApplicationFooter");fitChrome(footer);
    layout->addWidget(header);layout->addWidget(m_stack,1);layout->addWidget(footer);
    window->setCentralWidget(host);
    window->setMinimumSize(360,360);window->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    window->resize(QSize(1240,790).boundedTo(QGuiApplication::primaryScreen()->availableGeometry().size()));
    auto *backShortcut=new QShortcut(QKeySequence("Alt+Left"),window);
    connect(backShortcut,&QShortcut::activated,this,&QuickUi::back);
    qApp->installEventFilter(this);
}
QuickBridge *QuickUi::current() const { return m_pages.isEmpty()?nullptr:m_pages.last().bridge.data(); }
QuickUi::~QuickUi() {
    qApp->removeEventFilter(this);
    for(auto *view:m_main->findChildren<QQuickWidget *>())view->setSource(QUrl());
}
bool QuickUi::canGoBack() const { return m_pages.size()>1; }
QStringList QuickUi::path() const {
    QStringList result;
    for(int i=0;i<m_pages.size();++i)if(m_pages[i].bridge)result.append(i==0?tr("Menu główne"):m_pages[i].bridge->title());
    return result;
}
void QuickUi::back() {
    if(!canGoBack())return;
    QPointer<QWidget> controller=m_pages.last().controller;
    if(controller && !controller->close())m_closeRequested=false;
}
void QuickUi::push(QWidget *controller,QuickBridge *bridge) {
    for(const auto &page:m_pages)if(page.controller==controller)return;
    if(auto *previous=current();previous && previous->view()) {
        previous->view()->setEnabled(false);
        if(auto *root=previous->view()->rootObject())root->setVisible(false);
    }
    m_pages.append({controller,bridge});
    m_stack->addWidget(bridge->view());
    connect(controller,&QObject::destroyed,this,[this,controller]{removePage(controller);});
    showCurrent();
}
void QuickUi::removePage(QWidget *controller) {
    if(m_navigating)return;
    for(int i=m_pages.size()-1;i>0;--i)if(m_pages[i].controller==controller || !m_pages[i].controller) {
        const bool top=i==m_pages.size()-1;
        const auto page=m_pages.takeAt(i);
        if(page.bridge && page.bridge->view()) {
            page.bridge->suspend();m_stack->removeWidget(page.bridge->view());page.bridge->view()->hide();
        }
        if(top)showCurrent();
        if(m_closeRequested)QTimer::singleShot(0,this,[this]{if(canGoBack())back();else m_main->close();});
        return;
    }
}
void QuickUi::showCurrent() {
    auto *bridge=current();if(!bridge || !bridge->view())return;
    m_navigating=true;
    bridge->resume();bridge->refresh();
    auto *view=bridge->view();m_stack->setCurrentWidget(view);view->setEnabled(true);
    if(auto *root=view->rootObject())root->setVisible(true);
    view->show();view->setFocus(Qt::OtherFocusReason);
    m_navigating=false;emit navigationChanged();
    QTimer::singleShot(0,this,[this]{if(auto *bridge=current();bridge && bridge->view()){if(!m_main->isActiveWindow())m_main->activateWindow();bridge->view()->setFocus(Qt::OtherFocusReason);}});
}
void QuickUi::showAppearance() {
    if(m_appearancePopup){m_appearancePopup->raise();return;}
    auto *popup=new QWidget(m_main,Qt::Popup);popup->setProperty("sjQuickUtility",true);popup->setAttribute(Qt::WA_DeleteOnClose);
    m_appearancePopup=popup;
    auto *layout=new QVBoxLayout(popup);layout->setContentsMargins(0,0,0,0);
    auto *view=makeView(popup,m_pages.first().bridge,m_appearance,this,"qrc:/ui/AppearancePanel.qml",false);layout->addWidget(view);
    popup->resize(320,qMin(view->rootObject()?qCeil(view->rootObject()->implicitHeight()):360,qMax(180,m_main->height()-24)));
    const auto bounds=m_main->frameGeometry();popup->move(bounds.right()-popup->width()-12,qMin(bounds.top()+70,bounds.bottom()-popup->height()-12));popup->show();
}
void QuickUi::closeAppearance() { if(m_appearancePopup)m_appearancePopup->close(); }
bool QuickUi::eventFilter(QObject *object,QEvent *event) {
    // Polish precedes native mapping. Suppress the controller window before its
    // first Show, rather than moving an already-opening native dialog away.
    if(event->type()==QEvent::Polish)
        if(auto *widget=qobject_cast<QWidget *>(object);isTaskController(widget))keepControllerOffscreen(widget);
    if(object==m_main && event->type()==QEvent::Resize) {
        for(auto *window:QApplication::topLevelWidgets())if(window->isVisible()&&window->property("sjQuickTransient").toBool())requestTransientFit(window);
        if(auto *header=m_main->findChild<QQuickWidget *>("quickApplicationHeader");header && header->rootObject())
            header->rootObject()->setProperty("compactHeight",m_main->height()<720);
    }
    if(event->type()==QEvent::Resize || event->type()==QEvent::Show) {
        if(auto *widget=qobject_cast<QWidget *>(object);widget && widget->property("sjQuickTransient").toBool())
            if(auto *bridge=widget->findChild<QuickBridge *>(QString(),Qt::FindDirectChildrenOnly);bridge && bridge->view()) {
                bridge->view()->setGeometry(widget->rect());bridge->view()->raise();
            }
    }
    if(event->type()==QEvent::Resize || event->type()==QEvent::Show || event->type()==QEvent::LayoutRequest)
        if(auto *widget=qobject_cast<QWidget *>(object);widget && widget->property("sjQuickTransient").toBool())requestTransientFit(widget);
    if(object==m_main && event->type()==QEvent::Close && canGoBack()) {
        static_cast<QCloseEvent *>(event)->ignore();m_closeRequested=true;back();return true;
    }
    if(event->type()==QEvent::LanguageChange && !m_translationPending) {
        m_translationPending=true;
        QTimer::singleShot(0,this,[this] {
            m_translationPending=false;
            for(auto *window:QApplication::topLevelWidgets()) {
                for(auto *view:window->findChildren<QQuickWidget *>())view->engine()->retranslate();
                for(auto *table:window->findChildren<QuickTableModel *>())table->invalidateText();
                for(auto *roster:window->findChildren<QuickRoster *>())roster->invalidateText();
            }
            emit navigationChanged();
        });
    }
    auto *dialog=qobject_cast<QWidget *>(object);
    // Native widgets can send another Show while makeView is still constructing
    // the QQuickWidget. Its bridge has no view until that constructor returns.
    if(dialog && dialog->property("sjQuickInstalling").toBool())return false;
    if(event->type()==QEvent::Hide && dialog && dialog->property("sjQuickPage").toBool())removePage(dialog);
    if(event->type()==QEvent::Show && dialog && (dialog->isWindow() || dialog->property("sjQuickPage").toBool()) && !dialog->property("sjQuickUtility").toBool()
        && (qobject_cast<QDialog *>(dialog) || dialog->inherits("JumpDataDetailedInfoWindow"))) {
        if(qobject_cast<QFileDialog *>(dialog))return false;
        if(!dialog->property("sjQuickInstalled").toBool()) {
            dialog->setProperty("sjQuickInstalling",true);
            attach(dialog);
            dialog->setProperty("sjQuickInstalling",false);
        }
        else if(auto *bridge=dialog->findChild<QuickBridge *>(QString(),Qt::FindDirectChildrenOnly)) {
            bridge->resume();if(dialog->property("sjQuickPage").toBool())push(dialog,bridge);
        }
        if(dialog->property("sjQuickPage").toBool()) {
            // Keep the controller's local event loop and visibility contract while
            // its view lives in the application stack; no native dialog is mapped.
            keepControllerOffscreen(dialog);
        } else if(dialog->windowHandle() && m_main->windowHandle())dialog->windowHandle()->setTransientParent(m_main->windowHandle());
    }
    return false;
}
void QuickUi::requestTransientFit(QWidget *dialog) {
    if(!dialog || dialog->property("sjQuickSizing").toBool() || dialog->property("sjQuickFitPending").toBool())return;
    dialog->setProperty("sjQuickFitPending",true);const QPointer<QWidget> guarded=dialog;
    QTimer::singleShot(0,this,[this,guarded]{if(guarded){guarded->setProperty("sjQuickFitPending",false);fitTransient(guarded);}});
}
void QuickUi::fitTransient(QWidget *dialog) {
    if(!dialog || dialog->property("sjQuickSizing").toBool())return;
    auto *bridge=dialog->findChild<QuickBridge *>(QString(),Qt::FindDirectChildrenOnly);
    if(!bridge || !bridge->view() || !bridge->view()->rootObject())return;
    dialog->setProperty("sjQuickSizing",true);
    auto *screen=m_main->screen();const auto area=screen?screen->availableGeometry():QGuiApplication::primaryScreen()->availableGeometry();
    const QSize limit(qMax(280,qMin(area.width()-32,m_main->width()-24)),qMax(180,qMin(area.height()-48,m_main->height()-40)));
    if(dialog->layout())dialog->layout()->setSizeConstraint(QLayout::SetNoConstraint);
    const QSize minimum(qMin(320,limit.width()),qMin(180,limit.height()));
    if(dialog->minimumSize()!=minimum)dialog->setMinimumSize(minimum);
    if(dialog->maximumSize()!=limit)dialog->setMaximumSize(limit);
    const QSize desired(qMin(600,limit.width()),qBound(qMin(220,limit.height()),qCeil(bridge->view()->rootObject()->implicitHeight()),limit.height()));
    if(dialog->size()!=desired)dialog->resize(desired);
    const auto center=m_main->frameGeometry().center();QPoint position=center-QPoint(desired.width()/2,desired.height()/2);
    position.setX(qBound(area.left(),position.x(),area.right()-desired.width()+1));position.setY(qBound(area.top(),position.y(),area.bottom()-desired.height()+1));if(dialog->pos()!=position)dialog->move(position);
    if(bridge->view()->geometry()!=dialog->rect())bridge->view()->setGeometry(dialog->rect());dialog->setProperty("sjQuickSizing",false);
}
void QuickUi::attach(QWidget *dialog) {
    const bool transient=qobject_cast<QMessageBox *>(dialog) || qobject_cast<QInputDialog *>(dialog) || qobject_cast<QProgressDialog *>(dialog);
    if(!dialog->layout() && !transient)return;
    dialog->setProperty("sjQuickInstalled",true);
    dialog->setProperty("sjQuickPage",!transient);
    if(transient) {
        // Qt's standard dialogs keep private pointers to their native layout.
        // Preserve that layout and present the Quick scene above its controls.
        dialog->setProperty("sjQuickTransient",true);
        if(dialog->layout())dialog->layout()->setSizeConstraint(QLayout::SetNoConstraint);
        for(auto *label:dialog->findChildren<QLabel *>())label->setWordWrap(true);
        auto *bridge=new QuickBridge(dialog,dialog,m_appearance);
        auto *view=makeView(dialog,bridge,m_appearance,this,"qrc:/ui/ModalScreen.qml");
        dialog->setMinimumSize(280,180);dialog->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
        dialog->resize(560,300);
        const QPointer<QWidget> guarded=dialog;
        const auto fit=[this,guarded]{if(guarded)requestTransientFit(guarded);};
        if(view->rootObject())connect(view->rootObject(),&QQuickItem::implicitHeightChanged,this,fit);
        fit();
        view->setGeometry(dialog->rect());view->show();view->raise();view->setFocus();
        if(auto *modal=qobject_cast<QDialog *>(dialog))connect(modal,&QDialog::finished,bridge,[bridge]{bridge->suspend();});
        return;
    }
    auto *legacy=new QWidget(dialog);legacy->setObjectName("widgetsContent");legacy->setLayout(dialog->layout());legacy->resize(1440,900);
    auto *layout=new QVBoxLayout(dialog);layout->setContentsMargins(0,0,0,0);
    auto *bridge=new QuickBridge(dialog,legacy,m_appearance);
    makeView(m_stack,bridge,m_appearance,this,"qrc:/ui/Screen.qml");
    legacy->hide();dialog->setStyleSheet(QString());
    auto *owner=dialog->parentWidget();
    dialog->setProperty("sjQuickOwner",QVariant::fromValue(static_cast<QObject *>(owner)));
    keepControllerOffscreen(dialog);
    if(!m_main->isVisible())m_main->show();
    push(dialog,bridge);
    if(auto *modal=qobject_cast<QDialog *>(dialog))connect(modal,&QDialog::finished,bridge,[bridge]{bridge->suspend();});
    bridge->refresh();
}
