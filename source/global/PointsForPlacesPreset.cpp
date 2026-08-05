#include "PointsForPlacesPreset.h"
#include <QJsonArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonArray>
#include <QMessageBox>
#include <QByteArray>
#include <QFile>
#include "IdentifiableObjectsStorage.h"

PointsForPlacesPreset::PointsForPlacesPreset(QString name) : name(name)
{

}

QMap<int, double> PointsForPlacesPreset::getPointsForPlaces() const
{
    return pointsForPlaces;
}

QMap<int, double> &PointsForPlacesPreset::getPointsForPlacesReference()
{
    return pointsForPlaces;
}

void PointsForPlacesPreset::setPointsForPlaces(const QMap<int, double> &newPointsForPlaces)
{
    pointsForPlaces = newPointsForPlaces;
}

QJsonObject PointsForPlacesPreset::getJsonObject(PointsForPlacesPreset &preset)
{
    QJsonObject object;
    object.insert("name", preset.getName());
    QJsonArray pointsForPlacesArray;
    for(auto key : preset.getPointsForPlacesReference().keys()){
        pointsForPlacesArray.push_back(preset.getPointsForPlacesReference().value(key));
    }
    object.insert("pointsForPlaces", pointsForPlacesArray);
    return object;
}

QVector<PointsForPlacesPreset> PointsForPlacesPreset::getPointsForPlacesPresetsVectorFromJson(const QByteArray &bytes)
{
    QVector<PointsForPlacesPreset> presets;

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
    if(error.error != QJsonParseError::NoError)
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Błąd przy wczytytywaniu presetów punktów za miejsca", "Nie udało się wczytać presetów z pliku userData/GlobalDatabase/globalPointsForPlacesPresets.json\nTreść błędu: " + error.errorString(),  QMessageBox::StandardButton::Ok);
        message.setModal(true);
        message.exec();
        return presets;
    }
    QJsonObject object = document.object();
    QJsonValue value = object.value("pointsForPlacesPresets");
    if(!value.isArray())
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Błąd przy wczytytywaniu presetów punktów za miejsca","Prawdopodobnie w tym pliku nie ma listy z presetami\nUpewnij się, że wybrałeś właściwy plik!");
        message.setModal(true);
        message.exec();
        return presets;
    }
    QJsonArray array = value.toArray();
    for(const auto & val : array){
        presets.push_back(PointsForPlacesPreset::getFromJson(val.toObject()));
    }
    return presets;
}

PointsForPlacesPreset PointsForPlacesPreset::getFromJson(QJsonObject json)
{
    PointsForPlacesPreset preset;
    preset.setName(json.value("name").toString());
    QJsonArray pointsForPlacesArray = json.value("pointsForPlaces").toArray();
    int i=1;
    for(const auto & val : pointsForPlacesArray){
        preset.getPointsForPlacesReference().insert(i, val.toInt());
        i++;
    }
    return preset;
}

bool PointsForPlacesPreset::saveToFile(QString dir)
{
    QJsonDocument document;
    QJsonObject mainObject;
    mainObject.insert("points-for-places-preset", PointsForPlacesPreset::getJsonObject(*this));
    document.setObject(mainObject);
    QFile file(dir + getName() + ".json");
    file.open(QFile::WriteOnly | QFile::Text);
    file.resize(0);
    file.write(document.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

PointsForPlacesPreset PointsForPlacesPreset::loadFromJson(QString fileName)
{
    QFile file("userData/GlobalDatabase/pointsForPlacesPresets/" + fileName);
    file.open(QFile::ReadOnly | QFile::Text);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    QJsonObject object = doc.object().value("points-for-places-preset").toObject();
    PointsForPlacesPreset p = PointsForPlacesPreset::getFromJson(object);
    return p;
}

QString PointsForPlacesPreset::getName() const
{
    return name;
}

void PointsForPlacesPreset::setName(const QString &newName)
{
    name = newName;
}
