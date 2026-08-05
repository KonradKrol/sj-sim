#include "KOGroupInfoWidget.h"
#include "ui_KOGroupInfoWidgetui.h"

KOGroupInfoWidget::KOGroupInfoWidget(KOGroup *group, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KOGroupListView),
    group(group)
{
    ui->setupUi(this);

    jumpersListView = new DatabaseItemsListView(DatabaseItemsListView::SeasonJumpersItems, false, false, false, this);
    jumpersListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    jumpersListView->hideInstructions();
    jumpersListView->setShowItemsNumbers(true);
    jumpersListView->setSeasonJumpers(&group->getJumpersReference());
    jumpersListView->setupListModel();
    ui->verticalLayout_jumpersListView->addWidget(jumpersListView);

    connect(jumpersListView, &DatabaseItemsListView::listViewDoubleClicked, this, &KOGroupInfoWidget::jumpersListViewDoubleClicked);
}

KOGroupInfoWidget::~KOGroupInfoWidget()
{
    delete ui;
}

void KOGroupInfoWidget::updateWidget()
{
    jumpersListView->setSeasonJumpers(&group->getJumpersReference());
    jumpersListView->setupListModel();
    ui->label_groupNumber->setText(QString::number(group->getNumber()));
}

KOGroup *KOGroupInfoWidget::getGroup() const
{
    return group;
}

void KOGroupInfoWidget::setGroup(KOGroup *newGroup)
{
    group = newGroup;
}
