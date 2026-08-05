#include "Jumper.h"
#include "../global/CountryFlagsManager.h"
#include "../global/GlobalDatabase.h"

#include <QObject>
#include <QDebug>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonArray>
#include <QMessageBox>
#include <QRegularExpression>
#include <QByteArray>

Jumper::Jumper(const QString &name, const QString &surname, const QString &countryCode, const JumperSkills &skills) : name(name),
    surname(surname),
    countryCode(countryCode),
    jumperSkills(skills),
    Identifiable()
{
}

Jumper::~Jumper()
{
}

QPixmap Jumper::getImagePixmap()
{
    return QPixmap("jumpersImages/" + imageName);
}

QString Jumper::getTextInfo()
{
    return getNameAndSurname() + " (" + countryCode + ")";
}

QString Jumper::getTextForDiscord()
{
    return getNameAndSurname() + QString(" :flag_%1:").arg(GlobalDatabase::get()->getCountryByAlpha3(countryCode)->getAlpha2().toLower());
}

Jumper Jumper::loadByCSV(QStringList list)
{
    Jumper j;
    int i=0;
    for(auto & str : list)
    {
        if(i == 0){
            QStringList output = str.split(QRegularExpression("\\s"));
            j.setName(output.first());
            j.setSurname(str.mid(j.getName().length() + 1).trimmed());
            qDebug()<<"Name: "<<j.getName();
            qDebug()<<"Surname: "<<j.getSurname();
        }
        else if(i == 1)
            j.setCountryCode(str);
        else if(i == 2)
            j.getJumperSkillsPointer()->setTakeoffTechnique(str.toDouble());
        else if(i == 3)
            j.getJumperSkillsPointer()->setFlightTechnique(str.toDouble());
        else if(i == 4)
            j.getJumperSkillsPointer()->setFlightStyle(str.toDouble());
        else if(i == 5)
            j.getJumperSkillsPointer()->setForm(str.toDouble());
        else if(i == 6)
            j.getJumperSkillsPointer()->setLandingStyle(str.toDouble());
        else if(i == 7)
            j.getJumperSkillsPointer()->setJumpsEquality(str.toDouble());
        i++;
    }
    return j;
}

QString Jumper::getImageName() const
{
    return imageName;
}

void Jumper::setImageName(const QString &newImageName)
{
    imageName = newImageName;
}

double Jumper::getPersonalBest() const
{
    return personalBest;
}

void Jumper::setPersonalBest(double newPersonalBest)
{
    personalBest = newPersonalBest;
}

QJsonObject Jumper::getJsonObject(const Jumper & jumper)
{
    QJsonObject object;
    object.insert("id", jumper.getIDStr());
    object.insert("name", jumper.getName());
    object.insert("surname", jumper.getSurname());
    object.insert("country-code", jumper.getCountryCode().toUpper());
    object.insert("image-name", jumper.getImageName());
    object.insert("personal-best", jumper.getPersonalBest());

    object.insert("takeoff-technique", jumper.getJumperSkills().getTakeoffTechnique());
    object.insert("flight-technique", jumper.getJumperSkills().getFlightTechnique());
    object.insert("flight-style", jumper.getJumperSkills().getFlightStyle());
    object.insert("landing-style", jumper.getJumperSkills().getLandingStyle());
    object.insert("form", jumper.getJumperSkills().getForm());
    object.insert("jumps-equality", jumper.getJumperSkills().getJumpsEquality());

    QJsonArray characteristicsArray;
    QSet<Characteristic> cs = jumper.getJumperSkills().getCharacteristics();
    cs.detach();
    for(QSet<Characteristic>::iterator it = cs.begin(); it != cs.end(); ++it)
    {
        QJsonObject characteristicObject;
        characteristicObject.insert("type", it->getType());
        characteristicObject.insert("level", it->getLevel());
        characteristicsArray.push_back(characteristicObject);
    }
    object.insert("characteristics", characteristicsArray);
    return object;
}

Jumper Jumper::getFromJson(QJsonObject obj)
{
    Jumper jumper;
    jumper.setID(sole::rebuild(obj.value("id").toString().toStdString()));
    jumper.setName(obj.value("name").toString());
    jumper.setSurname(obj.value("surname").toString());
    jumper.setCountryCode(obj.value("country-code").toString());
    jumper.setImageName(obj.value("image-name").toString());
    jumper.setPersonalBest(obj.value("personal-best").toDouble());

    jumper.getJumperSkillsPointer()->setTakeoffTechnique(obj.value("takeoff-technique").toDouble());
    jumper.getJumperSkillsPointer()->setFlightTechnique(obj.value("flight-technique").toDouble());
    jumper.getJumperSkillsPointer()->setFlightStyle(obj.value("flight-style").toDouble());
    jumper.getJumperSkillsPointer()->setLandingStyle(obj.value("landing-style").toDouble());
    jumper.getJumperSkillsPointer()->setForm(obj.value("form").toDouble());
    jumper.getJumperSkillsPointer()->setJumpsEquality(obj.value("jumps-equality").toDouble());

    QJsonArray characteristicsArray = obj.value("characteristics").toArray();
    for(const auto & val : characteristicsArray){
        jumper.getJumperSkillsPointer()->insertCharacteristic(val.toObject().value("type").toString(), val.toObject().value("level").toDouble());
    }

    return jumper;
}

QVector<Jumper> Jumper::getVectorFromJson(const QByteArray &bytes)
{
    QVector<Jumper> jumpers;
    jumpers.clear();

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
    if(error.error != QJsonParseError::NoError)
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Błąd przy wczytytywaniu zawodników", "Nie udało się wczytać zawodników z pliku userData/GlobalDatabase/globalJumpers.json\nTreść błędu: " + error.errorString(), QMessageBox::StandardButton::Ok);
        message.setModal(true);
        message.exec();
        return jumpers;
    }
    QJsonObject object = document.object();
    QJsonValue value = object.value("jumpers");
    if(!value.isArray())
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Błąd przy wczytytywaniu zawodników", "Prawdopodobnie w tym pliku nie ma listy ze skoczkami\nUpewnij się, że wybrałeś właściwy plik!");
        message.setModal(true);
        message.exec();
        return jumpers;
    }
    QJsonArray array = value.toArray();

    for(const auto & val : array)
    {
        jumpers.push_back(Jumper::getFromJson(val.toObject()));
    }

    return jumpers;
}

JumperSkills Jumper::getJumperSkills() const
{
    return jumperSkills;
}

JumperSkills *Jumper::getJumperSkillsPointer() const
{
    return const_cast<JumperSkills*>(&jumperSkills);
}

void Jumper::setJumperSkills(const JumperSkills &newJumperSkills)
{
    jumperSkills = newJumperSkills;
}

QString Jumper::getName() const
{
    return name;
}

void Jumper::setName(const QString &newName)
{
    name = newName;
}

QString Jumper::getSurname() const
{
    return surname;
}

void Jumper::setSurname(const QString &newSurname)
{
    surname = newSurname;
}

QString Jumper::getNameAndSurname() const
{
    return name + " " + surname;
}

QString Jumper::getCountryCode() const
{
    return countryCode;
}

void Jumper::setCountryCode(const QString &newCountryCode)
{
    countryCode = newCountryCode;
}
