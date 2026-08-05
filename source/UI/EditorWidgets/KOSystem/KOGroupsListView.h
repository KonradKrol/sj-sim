#ifndef KOGROUPSLISTVIEW_H
#define KOGROUPSLISTVIEW_H

#include <QWidget>
#include <QVector>
#include "../../../competitions/KOSystem/KOGroup.h"
#include "KOGroupInfoWidget.h"

namespace Ui {
class KOGroupsListView;
}

class KOGroupsListView : public QWidget
{
    Q_OBJECT

public:
    explicit KOGroupsListView(QWidget *parent = nullptr);
    ~KOGroupsListView();

    void fillListLayout();

private:
    Ui::KOGroupsListView *ui;

    QVector<KOGroupInfoWidget *> groupWidgets;

    QVector<KOGroup> * KOGroups;
    QVector<Jumper *> * jumpersList;
public:
    QVector<KOGroup> *getKOGroups() const;
    void setKOGroups(QVector<KOGroup> *newKOGroups);
    QVector<Jumper *> *getJumpersList() const;
    void setJumpersList(QVector<Jumper *> *newJumpersList);
};

#endif // KOGROUPSLISTVIEW_H
