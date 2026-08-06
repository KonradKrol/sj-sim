#ifndef RESPONSIVEWINDOWUTILS_H
#define RESPONSIVEWINDOWUTILS_H

#include <QDialog>
#include <QGuiApplication>
#include <QLayout>
#include <QScreen>
#include <QScrollArea>

namespace ResponsiveWindowUtils
{
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
}

#endif // RESPONSIVEWINDOWUTILS_H
