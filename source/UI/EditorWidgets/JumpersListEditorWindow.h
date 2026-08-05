#ifndef JUMPERSLISTEDITORWINDOW_H
#define JUMPERSLISTEDITORWINDOW_H

#include <QDialog>
#include <QString>
#include <QAction>
#include "../DatabaseEditor/DatabaseItemsListView.h"

namespace Ui {
class JumpersListEditorWindow;
}

class JumpersListEditorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit JumpersListEditorWindow(QWidget *parent = nullptr, QString name = "", bool isDefault = false);
    ~JumpersListEditorWindow();

    QString getNameFromLineEdit();
    bool getIsDefaultFromCheckBox();
    void updateUnselectedJumpers();

signals:
    void submitted();

private slots:
    void on_pushButton_submit_clicked();

private:
    Ui::JumpersListEditorWindow *ui;
    QAction * removeAction;

    DatabaseItemsListView * selectedJumpersListView;
    DatabaseItemsListView * unselectedJumpersListView;
    QVector<Jumper *> selectedJumpers;
    QVector<Jumper *> unselectedJumpers;
    QVector<Jumper *> allJumpers;
public:
    QVector<Jumper *> getSelectedJumpers() const;
    QVector<Jumper *> & getSelectedJumpersReference();
    void setSelectedJumpers(const QVector<Jumper *> &newSelectedJumpers);
    QVector<Jumper *> getUnselectedJumpers() const;
    QVector<Jumper *> & getUnselectedJumpersReference();
    void setUnselectedJumpers(const QVector<Jumper *> &newUnselectedJumpers);
    QVector<Jumper *> getAllJumpers() const;
    void setAllJumpers(const QVector<Jumper *> &newAllJumpers);
    DatabaseItemsListView *getSelectedJumpersListView() const;
    DatabaseItemsListView *getUnselectedJumpersListView() const;
};

#endif // JUMPERSLISTEDITORWINDOW_H
