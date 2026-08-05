#ifndef CLASSIFICATIONRESULTSTABLEVIEW_H
#define CLASSIFICATIONRESULTSTABLEVIEW_H

#include <QWidget>
#include <QTableView>
#include "ClassificationResultsTableModel.h"
#include "../../../seasons/Classification.h"
#include "../../../seasons/SimulationSave.h"

namespace Ui {
class ClassificationResultsTableView;
}

class ClassificationResultsTableView : public QWidget
{
    Q_OBJECT

public:
    explicit ClassificationResultsTableView(bool allowShowDetails, Classification *classification, QWidget *parent = nullptr);
    ~ClassificationResultsTableView();

    void fillTable();

private:
    Ui::ClassificationResultsTableView *ui;

    ClassificationResultsTableModel * tableModel;
    Classification * classification;
    SimulationSave * save;
    bool allowShowDetails;
public:
    Classification *getClassification() const;
    void setClassification(Classification *newClassification);
    bool getAllowShowDetails() const;
    void setAllowShowDetails(bool newAllowShowDetails);
    QTableView * getTableView();
    SimulationSave *getSave() const;
    void setSave(SimulationSave *newSave);

private slots:
    void on_pushButton_webhook_clicked();
};

#endif // CLASSIFICATIONRESULTSTABLEVIEW_H
