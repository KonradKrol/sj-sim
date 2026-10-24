#include "KOGroupsListView.h"
#include "ui_KOGroupsListView.h"
#include "../../../utilities/functions.h"
#include "KOGroupEditorWidget.h"

KOGroupsListView::KOGroupsListView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KOGroupsListView)
{
    ui->setupUi(this);
}

KOGroupsListView::~KOGroupsListView()
{
    delete ui;
}

void KOGroupsListView::fillListLayout()
{
    const auto previous=groupWidgets;
    groupWidgets.clear();
    qDeleteAll(previous);
    if(!KOGroups)return;
    for(auto & group : *KOGroups)
    {
        KOGroupInfoWidget * widget = new KOGroupInfoWidget(&group, this);
        widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        widget->updateWidget();
        ui->verticalLayout_groupsWidgets->addWidget(widget);
        groupWidgets.push_back(widget);

        connect(widget, &KOGroupInfoWidget::jumpersListViewDoubleClicked, this, [this, widget](){
            KOGroupEditorWidget editor(this);
            editor.setJumpersList(jumpersList);
            editor.setGroup(widget->getGroup());
            editor.updateComboBoxesLayout();
            connect(&editor,&KOGroupEditorWidget::submitted,&editor,&KOGroupEditorWidget::accept);
            if(editor.exec()!=QDialog::Accepted)return;
            auto *target=editor.getGroup();
            const auto chosen=editor.getJumpersFromInputs();
            if(!target || chosen.size()!=target->getJumpersReference().size())return;
            for(auto *jumper:chosen){bool present=false;for(const auto &group:*KOGroups)if(group.getJumpers().contains(jumper)){present=true;break;}if(!present)return;}
            // Swap occupied slots without resizing groups or invalidating their models.
            for(int slot=0;slot<chosen.size();++slot){
                if(target->getJumpersReference()[slot]==chosen[slot])continue;
                bool found=false;
                for(auto &group:*KOGroups){
                    const int other=group.getJumpersReference().indexOf(chosen[slot]);
                    if(other<0)continue;
                    qSwap(target->getJumpersReference()[slot],group.getJumpersReference()[other]);found=true;break;
                }
                if(!found)return;
            }
            for(auto *info:groupWidgets)info->updateWidget();
        });
    }
}

QVector<Jumper *> *KOGroupsListView::getJumpersList() const
{
    return jumpersList;
}

void KOGroupsListView::setJumpersList(QVector<Jumper *> *newJumpersList)
{
    jumpersList = newJumpersList;
}

QVector<KOGroup> *KOGroupsListView::getKOGroups() const
{
    return KOGroups;
}

void KOGroupsListView::setKOGroups(QVector<KOGroup> *newKOGroups)
{
    KOGroups = newKOGroups;
}
