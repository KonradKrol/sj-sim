#ifndef EDITSTARTLISTWITHJUMPERSLISTSWINDOW_H
#define EDITSTARTLISTWITHJUMPERSLISTSWINDOW_H

#include <QDialog>
#include <QVector>
#include "JumpersListsListItemWidget.h"

namespace Ui {
class EditStartListWithJumpersListsWindow;
}

class EditStartListWithJumpersListsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit EditStartListWithJumpersListsWindow(QWidget *parent = nullptr);
    ~EditStartListWithJumpersListsWindow();

    QVector<QPair<SaveJumpersList *, int>> constructJumpersListsStatuses();
    void setupWidgets();

private:
    Ui::EditStartListWithJumpersListsWindow *ui;
    QVector<JumpersListsListItemWidget *> widgets;
    QVector<SaveJumpersList> * lists;
public:
    QVector<SaveJumpersList> *getLists() const;
    void setLists(QVector<SaveJumpersList> *newLists);
    QVector<JumpersListsListItemWidget *> getWidgets() const;
    void setWidgets(const QVector<JumpersListsListItemWidget *> &newWidgets);
private slots:
    void on_pushButton_submit_clicked();
};

#endif // EDITSTARTLISTWITHJUMPERSLISTSWINDOW_H
