#ifndef KOGROUPINFOWIDGET_H
#define KOGROUPINFOWIDGET_H

#include <QWidget>
#include "../../../competitions/KOSystem/KOGroup.h"
#include "../../DatabaseEditor/DatabaseItemsListView.h"

namespace Ui {
class KOGroupListView;
}

class KOGroupInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KOGroupInfoWidget(KOGroup * group, QWidget *parent = nullptr);
    ~KOGroupInfoWidget();

    void updateWidget();

signals:
    void jumpersListViewDoubleClicked();

private:
    Ui::KOGroupListView *ui;
    KOGroup * group;
    DatabaseItemsListView * jumpersListView;
public:
    KOGroup *getGroup() const;
    void setGroup(KOGroup *newGroup);
};

#endif // KOGROUPINFOWIDGET_H
