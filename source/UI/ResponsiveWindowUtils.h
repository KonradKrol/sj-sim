#ifndef RESPONSIVEWINDOWUTILS_H
#define RESPONSIVEWINDOWUTILS_H

#include <QAbstractButton>
#include <QAbstractSpinBox>
#include <QComboBox>
#include <QDialog>
#include <QGuiApplication>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QScreen>
#include <QScrollArea>
#include <QSettings>
#include <QTableView>
#include <QTimer>
#include <QTreeView>
#include <QWindow>

namespace ResponsiveWindowUtils
{
inline int boundedTextMinimum(QWidget *widget, const QString &text)
{
    const QString sample = text.isEmpty() ? QStringLiteral("000000") : text;
    return qBound(80, widget->fontMetrics().horizontalAdvance(sample) + 28, 220);
}

inline void relaxTextConstraints(QWidget *root)
{
    const auto relaxWidth = [](QWidget *widget, int minimumWidth) {
        widget->setMinimumWidth(minimumWidth);
        widget->setMaximumWidth(QWIDGETSIZE_MAX);
        QSizePolicy policy = widget->sizePolicy();
        policy.setHorizontalPolicy(QSizePolicy::Expanding);
        widget->setSizePolicy(policy);
    };

    for(QAbstractButton *button : root->findChildren<QAbstractButton *>()) {
        if(!button->text().isEmpty() && button->minimumWidth() == button->maximumWidth()) {
            relaxWidth(button, boundedTextMinimum(button, button->text()));
            if(button->toolTip().isEmpty())
                button->setToolTip(button->text());
        }
    }
    for(QLineEdit *lineEdit : root->findChildren<QLineEdit *>()) {
        if(lineEdit->minimumWidth() == lineEdit->maximumWidth())
            relaxWidth(lineEdit, boundedTextMinimum(lineEdit, lineEdit->placeholderText()));
    }
    for(QComboBox *comboBox : root->findChildren<QComboBox *>()) {
        if(comboBox->minimumWidth() == comboBox->maximumWidth()) {
            relaxWidth(comboBox, boundedTextMinimum(comboBox, comboBox->currentText()));
            comboBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
            comboBox->setMinimumContentsLength(8);
        }
    }
    for(QAbstractSpinBox *spinBox : root->findChildren<QAbstractSpinBox *>()) {
        if(spinBox->minimumWidth() == spinBox->maximumWidth())
            relaxWidth(spinBox, boundedTextMinimum(spinBox, QStringLiteral("000000")));
    }
    for(QLabel *label : root->findChildren<QLabel *>()) {
        if(!label->text().isEmpty() && label->minimumWidth() == label->maximumWidth()) {
            relaxWidth(label, 0);
            label->setWordWrap(true);
            if(label->toolTip().isEmpty())
                label->setToolTip(label->text());
        }
    }
}

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
            QTimer::singleShot(0, this, [this](){ ensureVisible(window->screen()); });
            QTimer::singleShot(50, this, [this](){ ensureVisible(window->screen()); });
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
        const QMargins frameMargins = window->windowHandle() != nullptr
            ? window->windowHandle()->frameMargins()
            : QMargins();
        const QRect currentFrame = window->frameGeometry();
        const int horizontalFrame = qMax(frameMargins.left() + frameMargins.right(),
                                         currentFrame.width() - window->geometry().width());
        const int verticalFrame = qMax(frameMargins.top() + frameMargins.bottom(),
                                       currentFrame.height() - window->geometry().height());
        const QSize maximumClientSize(
            qMax(1, available.width() - horizontalFrame),
            qMax(1, available.height() - verticalFrame));
        window->resize(window->size().boundedTo(maximumClientSize));

        QRect frame = window->frameGeometry();
        if(frame.left() < available.left())
            frame.moveLeft(available.left());
        if(frame.top() < available.top())
            frame.moveTop(available.top());
        if(frame.right() > available.right())
            frame.moveRight(available.right());
        if(frame.bottom() > available.bottom())
            frame.moveBottom(available.bottom());
        window->move(frame.topLeft());
    }
};

inline void manageWindowGeometry(QWidget *window, const QString &settingsKey)
{
    relaxTextConstraints(window);
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
