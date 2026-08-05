#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>

extern const QString appVersion;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QCloseEvent;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setParentApplication(QApplication *newParentApplication);
    QApplication *getParentApplication() const;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_pushButton_singleJumps_clicked();
    void on_pushButton_databaseEdit_clicked();
    void on_pushButton_settings_clicked();

    void on_pushButton_singleCompetition_clicked();

    void on_pushButton_reportIssue_clicked();

    void on_pushButton_season_clicked();

    void on_pushButton_help_clicked();

    void on_pushButton_thanks_clicked();

private:
    Ui::MainWindow *ui;

    QApplication * parentApplication;
};
#endif // MAINWINDOW_H
