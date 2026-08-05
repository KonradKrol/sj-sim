#ifndef NEWSIMULATIONSAVECONFIGURATIONWINDOW_H
#define NEWSIMULATIONSAVECONFIGURATIONWINDOW_H

#include <QDialog>
#include <QString>
#include <QStringList>

namespace Ui {
class NewSimulationSaveConfigurationWindow;
}

class NewSimulationSaveConfigurationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NewSimulationSaveConfigurationWindow(QStringList otherNames, QWidget *parent = nullptr);
    ~NewSimulationSaveConfigurationWindow();

    QString getNameFromInput() const;
    int getSeasonNumberFromInput() const;

private slots:
    void on_pushButton_submit_clicked();

private:
    Ui::NewSimulationSaveConfigurationWindow *ui;

    QStringList otherNames;
};

#endif // NEWSIMULATIONSAVECONFIGURATIONWINDOW_H
