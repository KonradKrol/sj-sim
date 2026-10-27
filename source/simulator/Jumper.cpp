#include "Jumper.h"
#include "../global/CountryCodes.h"
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
#include <QDir>
#include <QFileInfo>
#include <QUuid>

Jumper::Jumper(const QString &name, const QString &surname, const QString &countryCode, const JumperSkills &skills) : name(name),
    surname(surname),
    countryCode(CountryCodes::toFis(countryCode)),
    jumperSkills(skills),
    Identifiable()
{
}

Jumper::~Jumper()
{
}

QPixmap Jumper::getImagePixmap()
{
    const auto file=getImageFilePath();return file.isEmpty()?QPixmap():QPixmap(file);
}

QString Jumper::getImageFilePath() const
{
    if(!simulationProfile)return "jumpersImages/"+imageName;
    if(imageName.isEmpty() || simulationAssetDirectory.isEmpty())return {};
    const QString base=QFileInfo(simulationAssetDirectory).canonicalFilePath();
    const QString file=QFileInfo(QDir(simulationAssetDirectory).filePath(imageName)).canonicalFilePath();
#ifdef Q_OS_WIN
    const auto sensitivity=Qt::CaseInsensitive;
#else
    const auto sensitivity=Qt::CaseSensitive;
#endif
    return !base.isEmpty() && !file.isEmpty() && file.startsWith(base+"/",sensitivity) && QFileInfo(file).isFile()?file:QString();
}

void Jumper::setSimulationPackContext(const QString &packId,const QString &assetDirectory)
{
    simulationPackId=packId;simulationAssetDirectory=assetDirectory;
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
    if (jumper.getBirthYear() > 0) object.insert("birth-year", jumper.getBirthYear());
    if (jumper.getBirthDate().isValid()) object.insert("birth-date", jumper.getBirthDate().toString(Qt::ISODate));

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
    if(const auto profile=jumper.getSimulationProfile())object.insert("simulation-v2",profile->toJson());
    if(!jumper.simulationPackId.isEmpty())object.insert("simulation-pack-id",jumper.simulationPackId);
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
    jumper.setBirthYear(obj.value("birth-year").toInt());
    const QDate date = QDate::fromString(obj.value("birth-date").toString(), Qt::ISODate);
    if (date.isValid()) jumper.setBirthDate(date);

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

    if(obj.contains("simulation-v2")) {
        AthleteProfile profile;QString error;
        if(AthleteProfile::fromJson(obj["simulation-v2"].toObject(),profile,error))jumper.setSimulationProfile(profile,error);
        jumper.setSimulationPackContext(obj["simulation-pack-id"].toString(),{});
    }

    return jumper;
}

std::optional<AthleteProfile> Jumper::getSimulationProfile() const
{
    if(!simulationProfile)return std::nullopt;
    auto profile=*simulationProfile;
    profile.id=getIDStr();profile.name=name;profile.surname=surname;profile.country=countryCode;
    profile.personalBest=personalBest;profile.birthYear=getBirthYear();
    const QString oldImage=profile.photoPath.isEmpty()?profile.portrait:profile.photoPath;
    if(imageName!=oldImage) {profile.photoPath=imageName;profile.portrait.clear();}
    return profile;
}

bool Jumper::setSimulationProfile(const AthleteProfile &input,QString &error)
{
    AthleteProfile profile;if(!AthleteProfile::fromJson(input.toJson(),profile,error))return false;
    simulationProfile=profile;setID(sole::rebuild(profile.id.toStdString()));
    setName(profile.name);setSurname(profile.surname);setCountryCode(profile.country);
    setPersonalBest(profile.personalBest);setBirthYear(profile.birthYear);
    setImageName(profile.photoPath.isEmpty()?profile.portrait:profile.photoPath);
    return true;
}

bool Jumper::validateSimulationProfileJson(const QJsonObject &json,QString &error)
{
    if(!json.contains("simulation-v2"))return true;
    AthleteProfile profile;
    if(!json["simulation-v2"].isObject() || !AthleteProfile::fromJson(json["simulation-v2"].toObject(),profile,error))return false;
    if(profile.id!=json["id"].toString()) {error=QObject::tr("Niezgodne identyfikatory skoczka i nowego profilu.");return false;}
    if(json.contains("simulation-pack-id")) {
        const auto id=json["simulation-pack-id"].toString();
        if(!json["simulation-pack-id"].isString() || QUuid(id).isNull() || QUuid(id).toString(QUuid::WithoutBraces)!=id) {
            error=QObject::tr("Nieprawidłowy identyfikator paczki zdjęć zawodnika.");return false;
        }
    }
    return true;
}

QVector<Jumper> Jumper::getVectorFromJson(const QByteArray &bytes)
{
    QVector<Jumper> jumpers;
    jumpers.clear();

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
    if(error.error != QJsonParseError::NoError)
    {
        QMessageBox message(QMessageBox::Icon::Critical, QObject::tr("Błąd przy wczytytywaniu zawodników"), QObject::tr("Nie udało się wczytać zawodników z pliku userData/GlobalDatabase/globalJumpers.json\nTreść błędu: ") + error.errorString(), QMessageBox::StandardButton::Ok);
        message.setModal(true);
        message.exec();
        return jumpers;
    }
    QJsonObject object = document.object();
    QJsonValue value = object.value("jumpers");
    if(!value.isArray())
    {
        QMessageBox message(QMessageBox::Icon::Critical, QObject::tr("Błąd przy wczytytywaniu zawodników"), QObject::tr("Prawdopodobnie w tym pliku nie ma listy ze skoczkami\nUpewnij się, że wybrałeś właściwy plik!"));
        message.setModal(true);
        message.exec();
        return jumpers;
    }
    QJsonArray array = value.toArray();

    for(const auto & val : array)
    {
        QString profileError;
        if(!validateSimulationProfileJson(val.toObject(),profileError)) {
            QMessageBox::critical(nullptr,QObject::tr("Skoczkowie"),profileError);return {};
        }
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
    countryCode = CountryCodes::toFis(newCountryCode);
}
