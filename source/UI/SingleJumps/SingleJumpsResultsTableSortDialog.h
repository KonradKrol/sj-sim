#ifndef SINGLEJUMPSRESULTSTABLESORTDIALOG_H
#define SINGLEJUMPSRESULTSTABLESORTDIALOG_H

#include <QDialog>

namespace Ui {
class SingleJumpsResultsTableSortDialog;
}

class SingleJumpsResultsTableSortDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SingleJumpsResultsTableSortDialog(QWidget *parent = nullptr);
    ~SingleJumpsResultsTableSortDialog();

    enum SortBy{
        Distance,
        Points,
        Wind,
        Judges
    };
    enum SortType{
        Descending,
        Ascending
    };

    short sortBy();
    short sortType();

private slots:
    void on_pushButton_submit_clicked();

private:
    Ui::SingleJumpsResultsTableSortDialog *ui;
};

#endif // SINGLEJUMPSRESULTSTABLESORTDIALOG_H
