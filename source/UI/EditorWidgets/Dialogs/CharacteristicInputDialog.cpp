#include "CharacteristicInputDialog.h"
#include <QDebug>
#include "ui_CharacteristicInputDialog.h"

#include <QStringList>
#include <QStringListModel>

CharacteristicInputDialog::CharacteristicInputDialog(short action, short parentType, const QSet<Characteristic> & existingCharacteristics, Characteristic *characteristicToEdit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CharacteristicInputDialog),
    characteristicToEdit(characteristicToEdit),
    existingCharacteristics(existingCharacteristics),
    action(action),
    parentType(parentType)
{
    ui->setupUi(this);

    if(getAction() == Edit && this->characteristicToEdit == nullptr)
        this->characteristicToEdit = new Characteristic();

    switch(getAction())
    {
    case Add: ui->label_title->setText(tr("Dodaj cechę")); break;
    case Edit: ui->label_title->setText(tr("Edytuj cechę")); break;
    default: ui->label_title->setText(tr("<Błąd>")); break;
    }
    setWindowTitle(ui->label_title->text());
}

CharacteristicInputDialog::~CharacteristicInputDialog()
{
    delete ui;
}

QString CharacteristicInputDialog::getCharacteristicType() const
{
    return dirtyCharacteristicNames.at(ui->comboBox_type->currentIndex());
}

double CharacteristicInputDialog::getCharacteristicLevel() const
{
    return ui->doubleSpinBox_level->value();
}

Characteristic CharacteristicInputDialog::getCharacteristic() const
{
    return Characteristic(getCharacteristicLevel(), getCharacteristicType());
}

short CharacteristicInputDialog::getAction() const
{
    return action;
}

void CharacteristicInputDialog::setAction(short newAction)
{
    action = newAction;
}

Characteristic *CharacteristicInputDialog::getCharacteristicToEdit() const
{
    return characteristicToEdit;
}

void CharacteristicInputDialog::setCharacteristicToEdit(Characteristic *newCharacteristicToEdit)
{
    characteristicToEdit = newCharacteristicToEdit;
}

short CharacteristicInputDialog::getParentType() const
{
    return parentType;
}

void CharacteristicInputDialog::setParentType(short newParentType)
{
    parentType = newParentType;
}

QStringList CharacteristicInputDialog::getDirtyCharacteristicNames() const
{
    return dirtyCharacteristicNames;
}

void CharacteristicInputDialog::setDirtyCharacteristicNames(const QStringList &newDirtyCharacteristicNames)
{
    dirtyCharacteristicNames = newDirtyCharacteristicNames;
}

QSet<Characteristic> & CharacteristicInputDialog::getExistingCharacteristics()
{
    return existingCharacteristics;
}

void CharacteristicInputDialog::setExistingCharacteristics(const QSet<Characteristic> &newExistingCharacteristics)
{
    existingCharacteristics = newExistingCharacteristics;
}

void CharacteristicInputDialog::fillComboBox()
{
    qInfo()<<"\nvoid CharacteristicInputDialog::fillComboBox()";
    ui->comboBox_type->clear();
    QStringListModel * model = new QStringListModel;

    QStringList characteristics = Characteristic::characteristicTypesForSpecificParent(parentType, false);
    QStringList editableCharacteristics = characteristics;
    qInfo()<<"editableCharacteristics size: "<<editableCharacteristics.size();
    int i = 0;
    for(auto & characteristic : characteristics)
    {
        qInfo()<<"\ncharacteristics at "<<i<<" i:";
        for(auto & ch : editableCharacteristics){
            qInfo()<<"ch: "<<ch;
        }
        qInfo()<<"characteristic: "<<characteristic;
        qInfo()<<"contains: "<<existingCharacteristics.contains(Characteristic(characteristic));
        if(existingCharacteristics.contains(Characteristic(characteristic)))
        {
            int index = 0;
            int it = 0;
            for(it = 0; it < editableCharacteristics.size(); it++)
            {
                if(editableCharacteristics.at(it) == characteristic)
                    index = it;
            }
            qInfo()<<"characteristics size: "<<characteristics.size()<<", index to remove: "<<index;
            editableCharacteristics.removeAt(index);
        }
        i++;
    }
    qInfo()<<"After for loop";
    for(auto & characteristic : editableCharacteristics){
        dirtyCharacteristicNames.push_back(characteristic);
        characteristic = Characteristic::getTypeToDisplay(Characteristic(characteristic));
    }
    model->setStringList(editableCharacteristics);

    ui->comboBox_type->setModel(model);
}

void CharacteristicInputDialog::setComboBoxIndex(int index)
{
    ui->comboBox_type->setCurrentIndex(index);
}
