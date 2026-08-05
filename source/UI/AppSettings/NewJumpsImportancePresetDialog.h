#ifndef NEWJUMPSIMPORTANCEPRESETDIALOG_H
#define NEWJUMPSIMPORTANCEPRESETDIALOG_H

#include <QDialog>

namespace Ui {
class NewJumpsImportancePresetDialog;
}

class NewJumpsImportancePresetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewJumpsImportancePresetDialog(QWidget *parent = nullptr);
    ~NewJumpsImportancePresetDialog();

    QString name();
    double importance();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::NewJumpsImportancePresetDialog *ui;
};

#endif // NEWJUMPSIMPORTANCEPRESETDIALOG_H
