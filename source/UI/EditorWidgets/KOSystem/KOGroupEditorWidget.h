#ifndef KOGROUPEDITORWIDGET_H
#define KOGROUPEDITORWIDGET_H

#include <QDialog>
#include <QComboBox>
#include "../../../competitions/KOSystem/KOGroup.h"

namespace Ui {
class KOGroupEditorWidget;
}

class KOGroupEditorWidget : public QDialog
{
    Q_OBJECT

public:
    explicit KOGroupEditorWidget(QWidget *parent = nullptr);
    ~KOGroupEditorWidget();

    void updateGroupNumber();
    void updateComboBoxesLayout();
    KOGroup getKOGroupFromInputs();
    QVector<Jumper *> getJumpersFromInputs();

signals:
    void submitted();

private slots:
    void on_pushButton_submit_clicked();

private:
    Ui::KOGroupEditorWidget *ui;
    QVector<QComboBox *> comboBoxes;

    KOGroup * group;
    QVector<Jumper *> * jumpersList;
public:
    KOGroup *getGroup() const;
    void setGroup(KOGroup *newGroup);
    QVector<Jumper *> *getJumpersList() const;
    void setJumpersList(QVector<Jumper *> *newJumpersList);
};

#endif // KOGROUPEDITORWIDGET_H
