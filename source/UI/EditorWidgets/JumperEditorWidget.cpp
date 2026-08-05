#include "JumperEditorWidget.h"
#include "ui_JumperEditorWidget.h"

#include "../../global/GlobalDatabase.h"
#include "../../global/CountryFlagsManager.h"
#include "../../utilities/functions.h"

#include <QIcon>

JumperEditorWidget::JumperEditorWidget(Jumper *jumper, CharacteristicsEditor *characteristicsEditor, QWidget *parent) :
    jumper(jumper),
    characteristicsEditor(characteristicsEditor),
    QWidget(parent),
    ui(new Ui::JumperEditorWidget),
    formInstability(nullptr)
{
    ui->setupUi(this);

    if(this->characteristicsEditor == nullptr)
        this->characteristicsEditor = new CharacteristicsEditor(Characteristic::Jumper);

    ui->verticalLayout_characteristicsEditor->insertWidget(0, getCharacteristicsEditor());
    ui->label_countryFlag->setGeometry(ui->label_countryFlag->x(), ui->label_countryFlag->y(), CountryFlagsManager::getFlagPixmapSize().width(), CountryFlagsManager::getFlagPixmapSize().height());
}

JumperEditorWidget::~JumperEditorWidget()
{
    delete ui;
}

void JumperEditorWidget::resetJumperInputs()
{
    ui->lineEdit_name->setText("");
    ui->lineEdit_surname->setText("");
    ui->lineEdit_countryCode->setText("");
    ui->label_countryFlag->setPixmap(QPixmap());
    ui->lineEdit_img->clear();
    ui->doubleSpinBox_personalBest->setValue(0);
    ui->doubleSpinBox_takeoffTechnique->setValue(0);
    ui->doubleSpinBox_flightTechnique->setValue(0);
    ui->comboBox_flightStyle->setCurrentIndex(0);
    ui->doubleSpinBox_landingStyle->setValue(0);
    ui->doubleSpinBox_form->setValue(0);
    ui->doubleSpinBox_jumpsEquality->setValue(0);
    ui->doubleSpinBox_formInstability->setValue(0);
    characteristicsEditor->setCharacteristics(QSet<Characteristic>());
}

void JumperEditorWidget::fillJumperInputs()
{
    if(jumper == nullptr){
        qDebug()<<"Jumper is nullptr!";
        return;
    }
    ui->lineEdit_name->setText(jumper->getName());
    ui->lineEdit_surname->setText(jumper->getSurname());
    ui->lineEdit_countryCode->setText(jumper->getCountryCode());
    if(GlobalDatabase::get()->getCountryByAlpha3(jumper->getCountryCode().toUpper()))
        ui->label_countryName->setText(GlobalDatabase::get()->getCountryByAlpha3(jumper->getCountryCode().toUpper())->getName());
    ui->label_countryFlag->setPixmap(CountryFlagsManager::getFlagPixmap(jumper->getCountryCode().toLower()).scaled(ui->label_countryFlag->size()));
    ui->lineEdit_img->setText(jumper->getImageName());
    ui->doubleSpinBox_personalBest->setValue(jumper->getPersonalBest());
    if(formInstability != nullptr){
        ui->doubleSpinBox_formInstability->setValue(*formInstability);
        ui->doubleSpinBox_formInstability->show();
        ui->label_formInstability->show();
    }
    else{
        ui->doubleSpinBox_formInstability->hide();
        ui->label_formInstability->hide();
    }

    ui->doubleSpinBox_takeoffTechnique->setValue(jumper->getJumperSkills().getTakeoffTechnique());
    ui->doubleSpinBox_flightTechnique->setValue(jumper->getJumperSkills().getFlightTechnique());
    ui->comboBox_flightStyle->setCurrentIndex(jumper->getJumperSkills().getFlightStyle());
    ui->doubleSpinBox_landingStyle->setValue(jumper->getJumperSkills().getLandingStyle());
    ui->doubleSpinBox_form->setValue(jumper->getJumperSkills().getForm());
    ui->doubleSpinBox_jumpsEquality->setValue(jumper->getJumperSkills().getJumpsEquality());
    characteristicsEditor->setCharacteristics(jumper->getJumperSkills().getCharacteristics());

    emit ui->lineEdit_img->textChanged(jumper->getImageName());
}
Jumper JumperEditorWidget::getJumperFromWidgetInput() const
{
    Jumper jumper;
    jumper.setName(ui->lineEdit_name->text());
    jumper.setSurname(ui->lineEdit_surname->text());
    jumper.setCountryCode(ui->lineEdit_countryCode->text());
    jumper.setImageName(ui->lineEdit_img->text());
    jumper.setPersonalBest(ui->doubleSpinBox_personalBest->value());

    if(formInstability != nullptr){
        *formInstability = ui->doubleSpinBox_formInstability->value();
        ui->doubleSpinBox_formInstability->show();
        ui->label_formInstability->show();
    }
    else{
        ui->doubleSpinBox_formInstability->hide();
        ui->label_formInstability->hide();
    }

    jumper.getJumperSkillsPointer()->setTakeoffTechnique(ui->doubleSpinBox_takeoffTechnique->value());
    jumper.getJumperSkillsPointer()->setFlightTechnique(ui->doubleSpinBox_flightTechnique->value());
    jumper.getJumperSkillsPointer()->setFlightStyle(ui->comboBox_flightStyle->currentIndex());
    jumper.getJumperSkillsPointer()->setLandingStyle(ui->doubleSpinBox_landingStyle->value());
    jumper.getJumperSkillsPointer()->setForm(ui->doubleSpinBox_form->value());
    jumper.getJumperSkillsPointer()->setJumpsEquality(ui->doubleSpinBox_jumpsEquality->value());
    jumper.getJumperSkillsPointer()->setCharacteristics(characteristicsEditor->getCharacteristics());
    return jumper;
}

void JumperEditorWidget::removeSubmitButton()
{
    disconnect(ui->pushButton_submit, &QPushButton::clicked, this, &JumperEditorWidget::on_pushButton_submit_clicked);
    //ui->verticalLayout_characteristicsEditor->removeWidget(ui->pushButton_submit);
    delete ui->pushButton_submit;
}

void JumperEditorWidget::setShowForm(bool ok)
{
    ui->doubleSpinBox_form->setHidden(!ok);
    ui->label_form->setHidden(!ok);
    ui->label_formRange->setHidden(!ok);
}

Jumper *JumperEditorWidget::getJumper() const
{
    return jumper;
}

void JumperEditorWidget::setJumper(Jumper *newJumper)
{
    jumper = newJumper;
}

CharacteristicsEditor *JumperEditorWidget::getCharacteristicsEditor() const
{
    return characteristicsEditor;
}

void JumperEditorWidget::setCharacteristicsEditor(CharacteristicsEditor *newCharacteristicsEditor)
{
    characteristicsEditor = newCharacteristicsEditor;
}

double JumperEditorWidget::getFormInstabilityFromUI()
{
    return ui->doubleSpinBox_formInstability->value();
}

void JumperEditorWidget::on_lineEdit_countryCode_textChanged(const QString &arg1)
{
    if(arg1.length() > 2)
        ui->label_countryFlag->setPixmap(CountryFlagsManager::getFlagPixmap(ui->lineEdit_countryCode->text().toLower()).scaled(CountryFlagsManager::getFlagPixmapSize()));
}

void JumperEditorWidget::on_pushButton_submit_clicked()
{
    emit submitted();
}

void JumperEditorWidget::on_lineEdit_img_textChanged(const QString &arg1)
{
    QPixmap pixmap("jumpersImages/" + arg1);
    pixmap = pixmap.scaled(ui->label_imgPreview->size());
    ui->label_imgPreview->setPixmap(pixmap);
}

double *JumperEditorWidget::getFormInstability() const
{
    return formInstability;
}

void JumperEditorWidget::setFormInstability(double *newFormInstability)
{
    formInstability = newFormInstability;
}
