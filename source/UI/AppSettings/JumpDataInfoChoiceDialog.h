#ifndef JUMPDATAINFOCHOICEDIALOG_H
#define JUMPDATAINFOCHOICEDIALOG_H

#include <QDialog>
#include "../../global/JumpDataInfoChoice.h"

namespace Ui {
class JumpDataInfoChoiceDialog;
}

class JumpDataInfoChoiceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JumpDataInfoChoiceDialog(QWidget *parent = nullptr);
    ~JumpDataInfoChoiceDialog();

    JumpDataInfoChoice getChoiceFromInputs();
    void fillInputs();

private slots:
    void on_pushButton_submit_clicked();

private:
    Ui::JumpDataInfoChoiceDialog *ui;
    JumpDataInfoChoice * choice;
public:
    JumpDataInfoChoice *getChoice() const;
    void setChoice(JumpDataInfoChoice *newChoice);
};

#endif // JUMPDATAINFOCHOICEDIALOG_H
