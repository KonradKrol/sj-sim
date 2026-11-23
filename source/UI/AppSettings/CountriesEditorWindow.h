#ifndef COUNTRIESEDITORWINDOW_H
#define COUNTRIESEDITORWINDOW_H

#include <QDialog>

class CountriesController;

namespace Ui {
class CountriesEditorWindow;
}

// Host of the countries screen (CountriesEditor.qml). Accepted when the player saves, so the
// caller writes the countries file; rejected on cancel, leaving the database unchanged.
class CountriesEditorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CountriesEditorWindow(QWidget *parent = nullptr);
    ~CountriesEditorWindow();

    void fill() {}
    CountriesController *controller() const { return m_controller; }

private:
    Ui::CountriesEditorWindow *ui;
    CountriesController *m_controller;
};

#endif // COUNTRIESEDITORWINDOW_H
