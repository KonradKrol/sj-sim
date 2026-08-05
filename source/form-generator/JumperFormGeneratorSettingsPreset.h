#ifndef JUMPERFORMGENERATORSETTINGSPRESET_H
#define JUMPERFORMGENERATORSETTINGSPRESET_H

#include <QString>
#include "JumperFormGeneratorSettings.h"
#include "../global/IdentifiableObjectsStorage.h"

class JumperFormGeneratorSettingsPreset
{
public:
    JumperFormGeneratorSettingsPreset(JumperFormGeneratorSettings settings = JumperFormGeneratorSettings());

    static QJsonObject getJsonObject(JumperFormGeneratorSettingsPreset preset);
    static JumperFormGeneratorSettingsPreset getFromJson(QJsonObject json);
    static QVector<JumperFormGeneratorSettingsPreset> getVectorFromJson(QByteArray json);

    bool saveToFile(QString dir = "");
    bool loadFromFile(QString fileName, QString dir = "");

private:
    JumperFormGeneratorSettings settings;
    QString name;
public:
    JumperFormGeneratorSettings getSettings() const;
    JumperFormGeneratorSettings & getSettingsReference();
    void setSettings(const JumperFormGeneratorSettings &newSettings);
    QString getName() const;
    void setName(const QString &newName);
};

#endif // JUMPERFORMGENERATORSETTINGSPRESET_H
