#ifndef RESPONSIVEWINDOWUTILS_H
#define RESPONSIVEWINDOWUTILS_H

#include <QDialog>
#include <QGuiApplication>
#include <QHeaderView>
#include <QLayout>
#include <QScreen>
#include <QScrollArea>
#include <QSettings>
#include <QTableView>
#include <QTimer>
#include <QTreeView>
#include <QWindow>

namespace ResponsiveWindowUtils
{
class WindowGeometryManager : public QObject
{
public:
    WindowGeometryManager(QWidget *window, const QString &settingsKey)
        : QObject(window), window(window), settingsKey("windowGeometry/" + settingsKey)
    {
        window->installEventFilter(this);
        restoreOrInitialize();
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if(watched != window)
            return QObject::eventFilter(watched, event);

        if(event->type() == QEvent::Show) {
            connectToWindowScreen();
            ensureVisible(window->screen());
        }
        else if(event->type() == QEvent::Hide || event->type() == QEvent::Close) {
            QSettings().setValue(settingsKey, window->saveGeometry());
        }
        return QObject::eventFilter(watched, event);
    }

private:
    QWidget *window;
    QString settingsKey;
    bool screenConnected = false;

    static QScreen *screenForRect(const QRect &rect)
    {
        QScreen *bestScreen = nullptr;
        int largestIntersection = 0;
        for(QScreen *screen : QGuiApplication::screens()) {
            const QRect intersection = screen->availableGeometry().intersected(rect);
            const int area = intersection.width() * intersection.height();
            if(area > largestIntersection) {
                largestIntersection = area;
                bestScreen = screen;
            }
        }
        return bestScreen;
    }

    void restoreOrInitialize()
    {
        const QByteArray savedGeometry = QSettings().value(settingsKey).toByteArray();
        const bool restored = !savedGeometry.isEmpty() && window->restoreGeometry(savedGeometry);
        QScreen *restoredScreen = restored ? screenForRect(window->frameGeometry()) : nullptr;
        if(restoredScreen != nullptr) {
            ensureVisible(restoredScreen);
            return;
        }

        QScreen *screen = window->parentWidget() != nullptr
            ? window->parentWidget()->screen()
            : QGuiApplication::primaryScreen();
        if(screen == nullptr)
            return;

        const QRect available = screen->availableGeometry();
        QSize targetSize(qRound(available.width() * 0.85), qRound(available.height() * 0.85));
        targetSize = targetSize.expandedTo(window->minimumSizeHint()).boundedTo(available.size());
        window->resize(targetSize);
        window->move(available.center() - QPoint(targetSize.width() / 2, targetSize.height() / 2));
    }

    void connectToWindowScreen()
    {
        if(screenConnected || window->windowHandle() == nullptr)
            return;
        screenConnected = true;
        QObject::connect(window->windowHandle(), &QWindow::screenChanged, this, [this](QScreen *screen){
            QTimer::singleShot(0, this, [this, screen](){ ensureVisible(screen); });
        });
    }

    void ensureVisible(QScreen *screen)
    {
        if(screen == nullptr || window->isMaximized() || window->isFullScreen())
            return;
        const QRect available = screen->availableGeometry();
        QRect geometry = window->frameGeometry();
        geometry.setSize(geometry.size().boundedTo(available.size()));
        if(geometry.left() < available.left())
            geometry.moveLeft(available.left());
        if(geometry.top() < available.top())
            geometry.moveTop(available.top());
        if(geometry.right() > available.right())
            geometry.moveRight(available.right());
        if(geometry.bottom() > available.bottom())
            geometry.moveBottom(available.bottom());
        window->resize(geometry.size());
        window->move(geometry.topLeft());
    }
};

inline void manageWindowGeometry(QWidget *window, const QString &settingsKey)
{
    new WindowGeometryManager(window, settingsKey);
}

inline QScrollArea *addScrollableContent(QDialog *dialog, QWidget *content)
{
    QScrollArea *scrollArea = new QScrollArea(dialog);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(content);
    dialog->layout()->addWidget(scrollArea);
    return scrollArea;
}

inline void fitToAvailableScreen(QDialog *dialog, const QSize &preferredSize = QSize())
{
    if(dialog->layout() != nullptr)
        dialog->layout()->activate();
    dialog->adjustSize();

    QScreen *screen = dialog->screen();
    if(screen == nullptr)
        screen = QGuiApplication::primaryScreen();
    if(screen == nullptr)
        return;

    const QSize availableSize = screen->availableGeometry().size() - QSize(32, 32);
    const QSize minimumSize = QSize(320, 240).boundedTo(availableSize);
    QSize targetSize = preferredSize.isValid() ? preferredSize : dialog->sizeHint();
    targetSize = targetSize.expandedTo(minimumSize).boundedTo(availableSize);

    dialog->setMinimumSize(minimumSize);
    dialog->setSizeGripEnabled(true);
    dialog->resize(targetSize);
}

inline void configureHeader(QHeaderView *header, int primaryColumn)
{
    header->setMinimumSectionSize(44);
    header->setStretchLastSection(false);
    header->setSectionResizeMode(QHeaderView::Interactive);

    const int sectionCount = header->count();
    if(sectionCount > 0) {
        header->setSectionResizeMode(0, QHeaderView::Fixed);
        header->resizeSection(0, 56);
    }
    if(primaryColumn >= 0 && primaryColumn < sectionCount)
        header->setSectionResizeMode(primaryColumn, QHeaderView::Stretch);
    if(sectionCount > 2 && primaryColumn != 2) {
        header->setSectionResizeMode(2, QHeaderView::Fixed);
        header->resizeSection(2, 90);
    }
    for(int section = 3; section < sectionCount; ++section)
        header->resizeSection(section, 120);
}

inline void configureTableColumns(QTableView *table, int primaryColumn = 1)
{
    table->setTextElideMode(Qt::ElideRight);
    table->setWordWrap(false);
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    configureHeader(table->horizontalHeader(), primaryColumn);
}

inline void configureTreeColumns(QTreeView *tree, int primaryColumn = 1)
{
    tree->setTextElideMode(Qt::ElideRight);
    tree->setWordWrap(false);
    tree->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    configureHeader(tree->header(), primaryColumn);
}
}

#endif // RESPONSIVEWINDOWUTILS_H
