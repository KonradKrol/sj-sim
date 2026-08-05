#include "CharacteristicsEditor.h"
#include "ui_CharacteristicsEditor.h"

#include "Dialogs/CharacteristicInputDialog.h"
#include "../../utilities/functions.h"

#include <QStringListModel>
#include <QMessageBox>

CharacteristicsEditor::CharacteristicsEditor(short parentType, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CharacteristicsEditor),
    parentType(parentType)
{
    ui->setupUi(this);
    model = new QStringListModel(this);
    ui->listView_characteristics->setModel(model);

    connect(this, &CharacteristicsEditor::characteristicsChanged, this, &CharacteristicsEditor::updateStrings);
}

CharacteristicsEditor::~CharacteristicsEditor()
{
    delete ui;
}

QStringList CharacteristicsEditor::getStrings() const
{
    return strings;
}

QSet<Characteristic> CharacteristicsEditor::getCharacteristics() const
{
    return characteristics;
}

void CharacteristicsEditor::setCharacteristics(const QSet<Characteristic> &newCharacteristics)
{
    characteristics = newCharacteristics;
    emit characteristicsChanged();
}

void CharacteristicsEditor::on_pushButton_add_clicked()
{
    qInfo()<<"void CharacteristicsEditor::on_pushButton_add_clicked():";
    //qInstallMessageHandler(MyFunctions::fileMessageHandler);
    qInfo("installMessageHandler");
    CharacteristicInputDialog dialog(CharacteristicInputDialog::Add);
    qInfo("CharacteristicInputDialog constructor");
    dialog.setParentType(parentType);
    qInfo("CharacteristicInputDialog setParentType");
    dialog.setModal(true);
    qInfo("CharacteristicInputDialog setModal");
    dialog.setExistingCharacteristics(characteristics);
    qInfo("CharacteristicInputDialog setExistingCharacteristics");
    dialog.fillComboBox();
    qInfo("CharacteristicInputDialog fillComboBox");
    if(dialog.exec() == QDialog::Accepted)
    {
        if(dialog.getCharacteristicLevel() != 0){
            characteristics.insert(dialog.getCharacteristic());
            emit characteristicsChanged();
        }
    }

}

void CharacteristicsEditor::updateStrings()
{
    strings.clear();
    dirtyStrings.clear();
    for(const auto & characteristic : qAsConst(characteristics))
    {
        strings.push_back(Characteristic::getTypeToDisplay(characteristic) + " (Poziom " + QString::number(characteristic.getLevel()) + ")");
        dirtyStrings.push_back(characteristic.getType());
    }
    model->setStringList(strings);
}

short CharacteristicsEditor::getParentType() const
{
    return parentType;
}

void CharacteristicsEditor::setParentType(short newParentType)
{
    parentType = newParentType;
}
// co chce zrobic?
// Najpierw robimy roboczego QSeta<Characteristic>. Dajemy tam wszystko z getCharsByParentType i odejmujemy te z existing
// Ustawić index comboBoxa w CharacteristicInputDialog na taki, jaki będzie dla nowo utworzonego QSeta dla charakterystyki w this.characteristics (funkcja find)
void CharacteristicsEditor::on_pushButton_edit_clicked()
{
    if(ui->listView_characteristics->selectionModel()->selectedRows().size() > 0){
        int index = ui->listView_characteristics->selectionModel()->selectedRows().first().row();
        CharacteristicInputDialog dialog(CharacteristicInputDialog::Edit);
        dialog.setParentType(parentType);
        dialog.setModal(true);
        dialog.setExistingCharacteristics(characteristics);
        dialog.getExistingCharacteristics().remove(Characteristic(dirtyStrings.at(index)));
        dialog.fillComboBox();

        /*QStringList tempCharacteristics = Characteristic::characteristicTypesForSpecificParent(getParentType(), false);
        for(auto & c : qAsConst(dialog.getExistingCharacteristics())){
            if(tempCharacteristics.contains(c.getType()))
                tempCharacteristics.remove(tempCharacteristics.indexOf(c.getType()));
        }
        QString ch = this->strings.at(index);
        dialog.setComboBoxIndex(tempCharacteristics.indexOf(ch));*/

        if(dialog.exec() == QDialog::Accepted)
        {
            if(dialog.getCharacteristicLevel() != 0){
                characteristics.remove(Characteristic(dirtyStrings.at(index)));
                characteristics.insert(dialog.getCharacteristic());
                emit characteristicsChanged();
            }
        }
    }
}

void CharacteristicsEditor::on_pushButton_delete_clicked()
{
    if(ui->listView_characteristics->selectionModel()->selectedRows().size() > 0){
        int index = ui->listView_characteristics->selectionModel()->selectedRows().at(0).row();
        QMessageBox * message = new QMessageBox(QMessageBox::Question, "Usunięcie cechy", "Czy na pewno chcesz usunąć tą cechę?");
        message->setAttribute(Qt::WA_DeleteOnClose, true);
        message->setStyleSheet("background-color: white; color: black;");
        message->addButton(tr("Nie"), QMessageBox::RejectRole);
        message->addButton(tr("Tak"), QMessageBox::AcceptRole);
        message->setModal(true);
        message->show();
        connect(message, &QMessageBox::accepted, this, [this, index](){
            characteristics.remove(Characteristic(dirtyStrings.at(index)));
            emit characteristicsChanged();
        });
    }
}