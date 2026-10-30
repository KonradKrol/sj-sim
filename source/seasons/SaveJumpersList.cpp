#include "SaveJumpersList.h"
#include <QJsonArray>
#include <QRegularExpression>
#include <QUuid>

SaveJumpersList::SaveJumpersList(QString name) : name(name), id(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    isDefault = false;
}

void SaveJumpersList::reassignId()
{
    id=QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QJsonObject SaveJumpersList::getJsonObject(const SaveJumpersList &jumpersList)
{
    QJsonObject object;
    object.insert("id", jumpersList.id);
    object.insert("name", jumpersList.getName());
    object.insert("is-default", jumpersList.getIsDefault());
    QJsonObject limits;
    for(auto it=jumpersList.countryLimits.cbegin();it!=jumpersList.countryLimits.cend();++it)limits.insert(it.key(),it.value());
    object.insert("country-limits",limits);
    QJsonArray jumpersArray;
    for(auto & jumper : jumpersList.getJumpers())
    {
        jumpersArray.push_back(jumper->getIDStr());
    }
    object.insert("jumpers-ids", jumpersArray);

    return object;
}

SaveJumpersList SaveJumpersList::getFromJson(QJsonObject object, IdentifiableObjectsStorage * storage)
{
    SaveJumpersList jumpersList;
    const QUuid storedId(object.value("id").toString());
    if(!storedId.isNull())jumpersList.id=storedId.toString(QUuid::WithoutBraces);
    jumpersList.setName(object.value("name").toString());
    jumpersList.setIsDefault(object.value("is-default").toBool());
    const auto limits=object.value("country-limits").toObject();
    for(auto it=limits.begin();it!=limits.end();++it){
        const auto value=it.value();
        if(value.isDouble() && value.toDouble()==value.toInt(-1))jumpersList.setCountryLimit(it.key(),value.toInt(-1));
    }
    QJsonArray jumpersArray = object.value("jumpers-ids").toArray();
    for(auto jumperID : jumpersArray)
    {
        jumpersList.getJumpersReference().push_back(static_cast<Jumper *>(storage->get(jumperID.toString())));
    }

    return jumpersList;
}

int SaveJumpersList::countryLimit(const QString &country) const
{
    return countryLimits.value(country.trimmed().toUpper(),-1);
}

bool SaveJumpersList::setCountryLimit(const QString &country,int limit)
{
    const auto code=country.trimmed().toUpper();
    static const QRegularExpression validCode(QStringLiteral("^[A-Z]{3}$"));
    if(!validCode.match(code).hasMatch() || limit < -1 || limit > 10000)return false;
    if(limit<0)countryLimits.remove(code);else countryLimits.insert(code,limit);
    return true;
}

void SaveJumpersList::setCountryLimits(const QMap<QString,int> &limits)
{
    const auto copy=limits;
    countryLimits.clear();
    for(auto it=copy.cbegin();it!=copy.cend();++it)setCountryLimit(it.key(),it.value());
}

bool SaveJumpersList::getIsDefault() const
{
    return isDefault;
}

void SaveJumpersList::setIsDefault(bool newIsDefault)
{
    isDefault = newIsDefault;
}

QVector<Jumper *> SaveJumpersList::getJumpers() const
{
    return jumpers;
}

QVector<Jumper *> &SaveJumpersList::getJumpersReference()
{
    return jumpers;
}

void SaveJumpersList::setJumpers(const QVector<Jumper *> &newJumpers)
{
    jumpers = newJumpers;
}

QString SaveJumpersList::getName() const
{
    return name;
}

void SaveJumpersList::setName(const QString &newName)
{
    name = newName;
}
