#ifndef CHARACTERISTICINPUTDIALOG_H
#define CHARACTERISTICINPUTDIALOG_H

#include <QDialog>
#include <QString>
#include <QSet>
#include "../../../simulator/Characteristic.h"

namespace Ui {
class CharacteristicInputDialog;
}

class CharacteristicInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CharacteristicInputDialog(short action = 0, short parentType = 0, const QSet<Characteristic> &existingCharacteristics = QSet<Characteristic>(), Characteristic * characteristicToEdit = nullptr, QWidget *parent = nullptr);
    ~CharacteristicInputDialog();

    QString getCharacteristicType() const;
    double getCharacteristicLevel() const;
    Characteristic getCharacteristic() const;

    enum Action{
        Add,
        Edit
    };

    void fillComboBox();
    void setComboBoxIndex(int index);

private:
    Ui::CharacteristicInputDialog *ui;
    Characteristic * characteristicToEdit;
    QSet<Characteristic> existingCharacteristics;

    QStringList dirtyCharacteristicNames;

    short action;
    short parentType;

public:
    short getAction() const;
    void setAction(short newAction);
    Characteristic *getCharacteristicToEdit() const;
    void setCharacteristicToEdit(Characteristic *newCharacteristicToEdit);
    short getParentType() const;
    void setParentType(short newParentType);
    QStringList getDirtyCharacteristicNames() const;
    void setDirtyCharacteristicNames(const QStringList &newDirtyCharacteristicNames);
    QSet<Characteristic> &getExistingCharacteristics();
    void setExistingCharacteristics(const QSet<Characteristic> &newExistingCharacteristics);
};

#endif // CHARACTERISTICINPUTDIALOG_H
