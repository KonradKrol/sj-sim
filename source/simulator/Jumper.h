#ifndef JUMPER_H
#define JUMPER_H

#include "../utilities/Identifiable.h"
#include "JumperSkills.h"
#include "../athletes/AthleteProfile.h"
#include <optional>

#include <QDate>
#include <QString>
#include <QPixmap>
#include <QObject>
#include <QJsonObject>

class Jumper : public Identifiable
{
public:
    Jumper(const QString &name = "", const QString &surname = "", const QString &countryCode = "", const JumperSkills & skills = JumperSkills());
    ~Jumper();

    QPixmap getImagePixmap();
    QString getTextInfo();
    QString getTextForDiscord();

    static Jumper loadByCSV(QStringList list);

private:
    QString name, surname;
    QString countryCode;
    QString imageName;
    double personalBest = 0;
    QDate birthDate;
    int birthYear = 0;

    JumperSkills jumperSkills;
    std::optional<AthleteProfile> simulationProfile;
public:
    static QJsonObject getJsonObject(const Jumper &jumper);
    static Jumper getFromJson(QJsonObject obj);
    static QVector<Jumper> getVectorFromJson(const QByteArray & bytes);
    static void setupJumpersFlagPixmaps(QVector<Jumper> & jumpers);

    QString getName() const;
    void setName(const QString &newName);
    QString getSurname() const;
    void setSurname(const QString &newSurname);
    QString getNameAndSurname() const;
    QString getCountryCode() const;
    void setCountryCode(const QString &newCountryCode);
    JumperSkills getJumperSkills() const;
    JumperSkills * getJumperSkillsPointer() const;
    void setJumperSkills(const JumperSkills &newJumperSkills);
    double getPersonalBest() const;
    void setPersonalBest(double newPersonalBest);
    QDate getBirthDate() const { return birthDate; }
    int getBirthYear() const { return birthDate.isValid() ? birthDate.year() : birthYear; }
    void setBirthDate(const QDate &date) { birthDate = date; birthYear = date.isValid() ? date.year() : 0; }
    void setBirthYear(int year) {
        birthYear = year > 0 && year <= 9999 ? year : 0;
        if (birthDate.year() != birthYear) birthDate = QDate();
    }
    int ageInYear(int year) const { return getBirthYear() > 0 && year >= getBirthYear() ? year - getBirthYear() : -1; }
    QString getImageName() const;
    void setImageName(const QString &newImageName);
    std::optional<AthleteProfile> getSimulationProfile() const;
    bool setSimulationProfile(const AthleteProfile &, QString &error);
    static bool validateSimulationProfileJson(const QJsonObject &, QString &error);
};

#endif // JUMPER_H
