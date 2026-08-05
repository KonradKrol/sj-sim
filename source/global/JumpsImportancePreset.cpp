#include "JumpsImportancePreset.h"

#include <QJsonArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonArray>
#include <QMessageBox>
#include <QByteArray>
#include <QFile>

JumpsImportancePreset::JumpsImportancePreset()
{

}

QString JumpsImportancePreset::getName() const
{
    return name;
}

void JumpsImportancePreset::setName(const QString &newName)
{
    name = newName;
}

double JumpsImportancePreset::getJumpsImportance() const
{
    return jumpsImportance;
}

void JumpsImportancePreset::setJumpsImportance(double newJumpsImportance)
{
    jumpsImportance = newJumpsImportance;
}

QJsonObject JumpsImportancePreset::getJsonObject(JumpsImportancePreset &preset)
{
    QJsonObject object;
    object.insert("name", preset.getName());
    object.insert("jumps-importance", preset.getJumpsImportance());
    return object;
}

JumpsImportancePreset JumpsImportancePreset::getFromJson(QJsonObject json)
{
    JumpsImportancePreset preset;
    preset.setName(json.value("name").toString());
    preset.setJumpsImportance(json.value("jumps-importance").toDouble());
    return preset;
}

bool JumpsImportancePreset::saveToFile(QString dir)
{
    QJsonDocument document;
    document.setObject(JumpsImportancePreset::getJsonObject(*this));
    QFile file(dir + getName() + ".json");
    file.open(QFile::WriteOnly | QFile::Text);
    file.resize(0);
    file.write(document.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

JumpsImportancePreset JumpsImportancePreset::loadFromJson(QString fileName)
{
    QFile file("userData/GlobalDatabase/jumpsImportancePresets/" + fileName);
    file.open(QFile::ReadOnly | QFile::Text);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    QJsonObject object = doc.object();
    JumpsImportancePreset p = JumpsImportancePreset::getFromJson(object);
    return p;
}


