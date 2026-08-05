#ifndef COUNTRIESEDITORWINDOW_H
#define COUNTRIESEDITORWINDOW_H

#include <QDialog>
#include <QAction>
#include "CountriesEditorTableModel.h"

namespace Ui {
class CountriesEditorWindow;
}

class CountriesEditorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CountriesEditorWindow(QWidget *parent = nullptr);
    ~CountriesEditorWindow();

    void fill();
    void sort();

private slots:
    void on_pushButton_submit_clicked();

private:
    Ui::CountriesEditorWindow *ui;
    CountriesEditorTableModel * model;

    QAction * action_insert;
    QAction * action_remove;
public:
    CountriesEditorTableModel *getModel() const;
    void setModel(CountriesEditorTableModel *newModel);
};

#endif // COUNTRIESEDITORWINDOW_H
