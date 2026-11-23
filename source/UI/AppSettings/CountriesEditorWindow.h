#ifndef COUNTRIESEDITORWINDOW_H
#define COUNTRIESEDITORWINDOW_H

#include <QDialog>
#include <QVector>
#include "CountriesEditorTableModel.h"

class QLineEdit;
class QPushButton;
class QSortFilterProxyModel;

namespace Ui {
class CountriesEditorWindow;
}

// Simple list of countries: search, rename by double-click, add a country with a short form
// and remove one that no athlete or hill uses. Changes are kept in a draft until "Zapisz".
class CountriesEditorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CountriesEditorWindow(QWidget *parent = nullptr);
    ~CountriesEditorWindow();

    void fill();
    CountriesEditorTableModel *getModel() const { return model; }

private slots:
    void on_pushButton_submit_clicked();

private:
    void addCountry();
    void removeSelectedCountry();
    int selectedRow() const;

    Ui::CountriesEditorWindow *ui;
    QVector<Country> draft;
    CountriesEditorTableModel *model=nullptr;
    QSortFilterProxyModel *proxy=nullptr;
    QLineEdit *search=nullptr;
    QPushButton *removeButton=nullptr;
};

#endif // COUNTRIESEDITORWINDOW_H
