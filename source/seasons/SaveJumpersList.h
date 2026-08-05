#ifndef SAVEJUMPERSLIST_H
#define SAVEJUMPERSLIST_H

#include <QVector>
#include <QJsonObject>
#include <QString>
#include "../simulator/Jumper.h"
#include "../global/IdentifiableObjectsStorage.h"

class SaveJumpersList
{
public:
    SaveJumpersList(QString name = "");

    static QJsonObject getJsonObject(const SaveJumpersList &jumpersList);
    static SaveJumpersList getFromJson(QJsonObject object, IdentifiableObjectsStorage *storage);

private:
    QVector<Jumper *> jumpers;
    QString name;
    bool isDefault;
public:
    QVector<Jumper *> getJumpers() const;
    QVector<Jumper *> & getJumpersReference();
    void setJumpers(const QVector<Jumper *> &newJumpers);
    QString getName() const;
    void setName(const QString &newName);
    bool getIsDefault() const;
    void setIsDefault(bool newIsDefault);
};

#endif // SAVEJUMPERSLIST_H
