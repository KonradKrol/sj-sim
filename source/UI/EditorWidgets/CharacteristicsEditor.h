#ifndef CHARACTERISTICSEDITOR_H
#define CHARACTERISTICSEDITOR_H

#include <QWidget>
#include <QSet>
#include <QStringList>
#include <QStringListModel>
#include "../../simulator/Characteristic.h"

namespace Ui {
class CharacteristicsEditor;
}

class CharacteristicsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit CharacteristicsEditor(short parentType = (-1), QWidget *parent = nullptr);
    ~CharacteristicsEditor();

    QStringList getStrings() const;

    QSet<Characteristic> getCharacteristics() const;
    void setCharacteristics(const QSet<Characteristic> &newCharacteristics);
    short getParentType() const;
    void setParentType(short newParentType);

signals:
    void characteristicsChanged();

private slots:
    void on_pushButton_add_clicked();
    void on_pushButton_edit_clicked();
    void on_pushButton_delete_clicked();

    void updateStrings();

private:
    Ui::CharacteristicsEditor *ui;

    QSet<Characteristic> characteristics;
    QStringList strings;
    QStringList dirtyStrings;
    QStringListModel * model;

    short parentType;
};

#endif // CHARACTERISTICSEDITOR_H
