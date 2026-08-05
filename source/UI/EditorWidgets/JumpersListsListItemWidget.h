#ifndef JUMPERSLISTSLISTITEMWIDGET_H
#define JUMPERSLISTSLISTITEMWIDGET_H

#include <QWidget>
#include "../../seasons/SaveJumpersList.h"

namespace Ui {
class JumpersListsListItemWidget;
}

class JumpersListsListItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit JumpersListsListItemWidget(SaveJumpersList * list, QWidget *parent = nullptr);
    ~JumpersListsListItemWidget();

    enum Status{
        Select,
        Unselect,
        Neutral
    };

signals:
    void selected();
    void unselected();

private slots:
    void on_pushButton_select_clicked();
    void on_pushButton_unselect_clicked();

private:
    Ui::JumpersListsListItemWidget *ui;
    int status;
    SaveJumpersList * list;
public:
    SaveJumpersList *getList() const;
    void setList(SaveJumpersList *newList);
    int getStatus() const;
    void setStatus(int newStatus);
};

#endif // JUMPERSLISTSLISTITEMWIDGET_H
