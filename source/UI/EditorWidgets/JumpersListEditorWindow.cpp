#include "JumpersListEditorWindow.h"
#include "ui_JumpersListEditorWindow.h"
#include "../../utilities/functions.h"

JumpersListEditorWindow::JumpersListEditorWindow(QWidget *parent, QString name, bool isDefault) :
    QDialog(parent),
    ui(new Ui::JumpersListEditorWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setText(name);
    ui->checkBox_default->setChecked(isDefault);

    selectedJumpersListView = new DatabaseItemsListView(DatabaseItemsListView::SeasonJumpersItems, false, false, true);
    selectedJumpersListView->setSeasonJumpers(&selectedJumpers);
    selectedJumpersListView->setupListModel();
    selectedJumpersListView->setRemoveKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_D);
    selectedJumpersListView->setUpKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_Up);
    selectedJumpersListView->setDownKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_Down);
    ui->verticalLayout_selected->addWidget(selectedJumpersListView);

    unselectedJumpersListView = new DatabaseItemsListView(DatabaseItemsListView::SeasonJumpersItems, false, false, true);
    unselectedJumpersListView->setSeasonJumpers(&unselectedJumpers);
    unselectedJumpersListView->setupListModel();
    ui->verticalLayout_unselected->addWidget(unselectedJumpersListView);

    connect(selectedJumpersListView, &DatabaseItemsListView::remove, this, [this](QVector<int> rows){
        std::sort(rows.begin(), rows.end());
        for(int i=0; i<rows.count(); i++)
        {
            int row = rows[i] - i;
            Jumper * jumper = selectedJumpers[row];

            selectedJumpers.remove(row);
            selectedJumpersListView->getListModel()->removeRow(row);

            unselectedJumpers.push_back(jumper);
            unselectedJumpersListView->getListModel()->insertRow(row);
            emit unselectedJumpersListView->getListModel()->dataChanged(unselectedJumpersListView->getListModel()->index(row), unselectedJumpersListView->getListModel()->index(row));
        }
    });

    connect(unselectedJumpersListView, &DatabaseItemsListView::remove, this, [this](QVector<int> rows){
        std::sort(rows.begin(), rows.end());
        for(int i=0; i<rows.count(); i++)
        {
            int row = rows[i] - i;
            Jumper * jumper = unselectedJumpers[row];

            unselectedJumpers.remove(row);
            unselectedJumpersListView->getListModel()->removeRow(row);

            selectedJumpers.push_back(jumper);
            selectedJumpersListView->getListModel()->insertRow(row);
            emit selectedJumpersListView->getListModel()->dataChanged(selectedJumpersListView->getListModel()->index(row), selectedJumpersListView->getListModel()->index(row));
        }
    });

    connect(this, &JumpersListEditorWindow::submitted, this, &JumpersListEditorWindow::accept);
}

JumpersListEditorWindow::~JumpersListEditorWindow()
{
    delete ui;
}

QString JumpersListEditorWindow::getNameFromLineEdit()
{
    return ui->lineEdit->text();
}

bool JumpersListEditorWindow::getIsDefaultFromCheckBox()
{
    return ui->checkBox_default->isChecked();
}

void JumpersListEditorWindow::updateUnselectedJumpers()
{
    unselectedJumpers.clear();
    for(auto & jumper : allJumpers)
    {
        if(selectedJumpers.contains(jumper) == false)
            unselectedJumpers.push_back(jumper);
    }
}

void JumpersListEditorWindow::on_pushButton_submit_clicked()
{
    emit submitted();
}

DatabaseItemsListView *JumpersListEditorWindow::getUnselectedJumpersListView() const
{
    return unselectedJumpersListView;
}

DatabaseItemsListView *JumpersListEditorWindow::getSelectedJumpersListView() const
{
    return selectedJumpersListView;
}

QVector<Jumper *> JumpersListEditorWindow::getSelectedJumpers() const
{
    return selectedJumpers;
}

QVector<Jumper *> &JumpersListEditorWindow::getSelectedJumpersReference()
{
    return selectedJumpers;
}

void JumpersListEditorWindow::setSelectedJumpers(const QVector<Jumper *> &newSelectedJumpers)
{
    selectedJumpers = newSelectedJumpers;
}

QVector<Jumper *> JumpersListEditorWindow::getUnselectedJumpers() const
{
    return unselectedJumpers;
}

QVector<Jumper *> &JumpersListEditorWindow::getUnselectedJumpersReference()
{
    return unselectedJumpers;
}

void JumpersListEditorWindow::setUnselectedJumpers(const QVector<Jumper *> &newUnselectedJumpers)
{
    unselectedJumpers = newUnselectedJumpers;
}

QVector<Jumper *> JumpersListEditorWindow::getAllJumpers() const
{
    return allJumpers;
}

void JumpersListEditorWindow::setAllJumpers(const QVector<Jumper *> &newAllJumpers)
{
    allJumpers = newAllJumpers;
}

