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
    for(auto & widget : groupWidgets)
    {
        MyFunctions::removeFromVector(groupWidgets, widget);
        ui->verticalLayout_groupsWidgets->removeWidget(widget);
        delete widget;
    }
    qDebug()<<"KOGROUPS COUNT: "<<KOGroups->count();
    for(auto & group : *KOGroups)
    {
        KOGroupInfoWidget * widget = new KOGroupInfoWidget(&group, this);
        widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        widget->updateWidget();
        ui->verticalLayout_groupsWidgets->addWidget(widget);
        groupWidgets.push_back(widget);

        connect(widget, &KOGroupInfoWidget::jumpersListViewDoubleClicked, this, [this, widget](){
            KOGroupEditorWidget * editor = new KOGroupEditorWidget(this);
            editor->setJumpersList(jumpersList);
            editor->setGroup(widget->getGroup());
            editor->updateComboBoxesLayout();
            connect(editor, &KOGroupEditorWidget::submitted, editor, &KOGroupEditorWidget::accept);
            if(editor->exec() == QDialog::Accepted)
            {
                KOGroup * oldGroup = editor->getGroup();
                for(auto & editorJumper : editor->getJumpersFromInputs())
                {
                    if(oldGroup->getJumpersReference().contains(editorJumper))
                    {

                    }
                    else{
                        //Szukamy miejsce editorJumpera
                        for(auto & group : *KOGroups)
                        {
                            for(auto & jumper : group.getJumpersReference())
                            {
                                if(editorJumper == jumper){
                                    int newJumperIndex = editor->getJumpersFromInputs().indexOf(editorJumper);
                                    Jumper * oldJumper = oldGroup->getJumpersReference()[newJumperIndex];
                                    jumper = oldJumper;
                                }
                            }
                        }
                    }
                }
                oldGroup->setJumpers(editor->getJumpersFromInputs());
            }
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
