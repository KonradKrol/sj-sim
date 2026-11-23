#include "Notice.h"

#include <QApplication>
#include <QEvent>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QPropertyAnimation>
#include <QSettings>
#include <QTimer>

namespace {
// Interface screens are drawn in the main window; task windows stay off screen.
QWidget *hostWindow()
{
    if(auto *active=QApplication::activeWindow();active && active->isVisible() && !active->testAttribute(Qt::WA_DontShowOnScreen))return active;
    for(auto *widget:QApplication::topLevelWidgets())
        if(qobject_cast<QMainWindow *>(widget) && widget->isVisible())return widget;
    return nullptr;
}

bool reducedMotion()
{
    return QSettings().value(QStringLiteral("quickUi/reducedMotion"),false).toBool();
}

// Keeps the notice centred above the bottom edge while the window is resized.
class Follower : public QObject
{
public:
    Follower(QLabel *label,QWidget *host) : QObject(label),label(label),host(host){host->installEventFilter(this);place();}
    void place()
    {
        if(!label || !host)return;
        label->adjustSize();
        const int bottom=qMax(24,host->height()/20);
        label->move((host->width()-label->width())/2,host->height()-label->height()-bottom);
        label->raise();
    }
protected:
    bool eventFilter(QObject *watched,QEvent *event) override
    {
        if(watched==host && event->type()==QEvent::Resize)place();
        return false;
    }
private:
    QPointer<QLabel> label;
    QPointer<QWidget> host;
};
}

void Notice::show(const QString &text)
{
    QWidget *host=hostWindow();
    if(!host)return;
    // One notice at a time: a newer confirmation replaces the previous one.
    for(auto *previous:host->findChildren<QLabel *>(QStringLiteral("sjNotice"),Qt::FindDirectChildrenOnly))previous->deleteLater();
    auto *label=new QLabel(text,host);
    label->setObjectName(QStringLiteral("sjNotice"));
    label->setAttribute(Qt::WA_TransparentForMouseEvents);
    label->setAccessibleName(text);
    label->setMaximumWidth(qMax(240,host->width()-64));
    label->setWordWrap(false);
    label->setStyleSheet(QStringLiteral("QLabel#sjNotice{background:#142e42;color:#f1f6ff;border-radius:18px;padding:9px 20px;"
                                        "border:1px solid rgba(216,238,133,140);font-size:14px;font-weight:600;}"));
    auto *follower=new Follower(label,host);
    label->show();
    follower->place();

    const bool animate=!reducedMotion();
    auto *opacity=new QGraphicsOpacityEffect(label);
    opacity->setOpacity(animate?0.0:1.0);
    label->setGraphicsEffect(opacity);
    if(animate){
        auto *fadeIn=new QPropertyAnimation(opacity,"opacity",label);
        fadeIn->setDuration(160);fadeIn->setStartValue(0.0);fadeIn->setEndValue(1.0);fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    }
    const QPointer<QLabel> guarded(label);
    QTimer::singleShot(2800,label,[guarded,opacity,animate]{
        if(!guarded)return;
        if(!animate){guarded->deleteLater();return;}
        auto *fadeOut=new QPropertyAnimation(opacity,"opacity",guarded);
        fadeOut->setDuration(260);fadeOut->setStartValue(1.0);fadeOut->setEndValue(0.0);
        QObject::connect(fadeOut,&QPropertyAnimation::finished,guarded,&QObject::deleteLater);
        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    });
}
