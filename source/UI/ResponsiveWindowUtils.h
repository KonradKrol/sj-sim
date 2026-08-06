#ifndef RESPONSIVEWINDOWUTILS_H
#define RESPONSIVEWINDOWUTILS_H

#include <QDialog>
#include <QGuiApplication>
#include <QHeaderView>
#include <QLayout>
#include <QScreen>
#include <QScrollArea>
#include <QTableView>
#include <QTreeView>

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
