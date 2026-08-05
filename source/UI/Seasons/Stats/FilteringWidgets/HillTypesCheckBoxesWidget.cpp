#include "HillTypesCheckBoxesWidget.h"
#include "ui_HillTypesCheckBoxesWidget.h"

HillTypesCheckBoxesWidget::HillTypesCheckBoxesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HillTypesCheckBoxesWidget)
{
    ui->setupUi(this);

    connect(ui->checkBox_small, &QCheckBox::stateChanged, this, &HillTypesCheckBoxesWidget::changed);
    connect(ui->checkBox_average, &QCheckBox::stateChanged, this, &HillTypesCheckBoxesWidget::changed);
    connect(ui->checkBox_normal, &QCheckBox::stateChanged, this, &HillTypesCheckBoxesWidget::changed);
    connect(ui->checkBox_large, &QCheckBox::stateChanged, this, &HillTypesCheckBoxesWidget::changed);
    connect(ui->checkBox_big, &QCheckBox::stateChanged, this, &HillTypesCheckBoxesWidget::changed);
    connect(ui->checkBox_flying, &QCheckBox::stateChanged, this, &HillTypesCheckBoxesWidget::changed);
}

HillTypesCheckBoxesWidget::~HillTypesCheckBoxesWidget()
{
    delete ui;
}

QSet<int> HillTypesCheckBoxesWidget::getHillTypesSet()
{
    QSet<int> types;
    if(ui->checkBox_small->isChecked() == true)
        types.insert(Hill::Small);
    if(ui->checkBox_average->isChecked() == true)
        types.insert(Hill::Average);
    if(ui->checkBox_normal->isChecked() == true)
        types.insert(Hill::Normal);
    if(ui->checkBox_large->isChecked() == true)
        types.insert(Hill::Large);
    if(ui->checkBox_big->isChecked() == true)
        types.insert(Hill::Big);
    if(ui->checkBox_flying->isChecked() == true)
        types.insert(Hill::Flying);

    return types;
}
