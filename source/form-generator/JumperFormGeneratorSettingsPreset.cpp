#include "JumperFormGeneratorSettingsPreset.h"
#include <QJsonValue>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QMessageBox>

JumperFormGeneratorSettingsPreset::JumperFormGeneratorSettingsPreset(JumperFormGeneratorSettings settings) : settings(settings)
{

}

QJsonObject JumperFormGeneratorSettingsPreset::getJsonObject(JumperFormGeneratorSettingsPreset preset)
{
    QJsonObject object;
    QJsonObject settingsObject;
    JumperFormGeneratorSettings * settings = &preset.getSettingsReference();
    settingsObject.insert("form-variability", settings->getFormVariability());
    settingsObject.insert("min-form", settings->getMinForm());
    settingsObject.insert("max-form", settings->getMaxForm());
    settingsObject.insert("form-bonus", settings->getFormBonus());
    object.insert("settings", settingsObject);
    object.insert("name", preset.getName());
    return object;
}

JumperFormGeneratorSettingsPreset JumperFormGeneratorSettingsPreset::getFromJson(QJsonObject json)
{
    JumperFormGeneratorSettingsPreset preset;
    preset.setName(json.value("name").toString());
    JumperFormGeneratorSettings * settings = &preset.getSettingsReference();
    QJsonObject object = json.value("settings").toObject();
    settings->setFormVariability(object.value("form-variability").toDouble());
    settings->setMinForm(object.value("min-form").toDouble());
    settings->setMaxForm(object.value("max-form").toDouble());
    settings->setFormBonus(object.value("form-bonus").toDouble());
    return preset;
}

QVector<JumperFormGeneratorSettingsPreset> JumperFormGeneratorSettingsPreset::getVectorFromJson(QByteArray json)
{
    QVector<JumperFormGeneratorSettingsPreset> presets;
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(json, &error);
    if(error.error != QJsonParseError::NoError)
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Błąd przy wczytytywaniu presetów generatora formy", "Treść błędu: " + error.errorString(),  QMessageBox::StandardButton::Ok);
        message.setModal(true);
        message.exec();
        return presets;
    }
    QJsonObject object = document.object();
    QJsonValue value = object.value("formGeneratorSettingsPresets");
    if(!value.isArray())
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Błąd przy wczytytywaniu generatora formy","Prawdopodobnie w tym pliku nie ma listy z presetami\nUpewnij się, że wybrałeś właściwy plik!");
        message.setModal(true);
        message.exec();
        return presets;
    }
    QJsonArray array = value.toArray();
    for(const auto & val : array)
    {
        presets.push_back(JumperFormGeneratorSettingsPreset::getFromJson(val.toObject()));
    }
    return presets;
}

bool JumperFormGeneratorSettingsPreset::saveToFile(QString dir)
{
    QJsonDocument document;
    QJsonObject mainObject;
    mainObject.insert("jumper-form-generator-settings-preset", JumperFormGeneratorSettingsPreset::getJsonObject(*this));
    document.setObject(mainObject);
    QFile file(dir + getName() + ".json");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Nie można otworzyć pliku z presetem generatora formy " + getName(), "Nie udało się otworzyć pliku " + dir + getName() + ".json" + "\nUpewnij się, że istnieje tam taki plik lub ma on odpowiednie uprawnienia",  QMessageBox::StandardButton::Ok);
        message.setModal(true);
        message.exec();
        return false;
    }
    file.resize(0);
    file.write(document.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

QString JumperFormGeneratorSettingsPreset::getName() const
{
    return name;
}

void JumperFormGeneratorSettingsPreset::setName(const QString &newName)
{
    name = newName;
}

JumperFormGeneratorSettings JumperFormGeneratorSettingsPreset::getSettings() const
{
    return settings;
}

JumperFormGeneratorSettings &JumperFormGeneratorSettingsPreset::getSettingsReference()
{
    return settings;
}

void JumperFormGeneratorSettingsPreset::setSettings(const JumperFormGeneratorSettings &newSettings)
{
    settings = newSettings;
}
