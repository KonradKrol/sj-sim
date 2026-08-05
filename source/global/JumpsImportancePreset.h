#ifndef JUMPSIMPORTANCEPRESET_H
#define JUMPSIMPORTANCEPRESET_H

#include <QString>
#include <QJsonObject>

class JumpsImportancePreset
{
public:
    JumpsImportancePreset();

private:
    QString name;
    double jumpsImportance;
public:
    QString getName() const;
    void setName(const QString &newName);
    double getJumpsImportance() const;
    void setJumpsImportance(double newJumpsImportance);
public:
    static QJsonObject getJsonObject(JumpsImportancePreset & preset);
    static QVector<JumpsImportancePreset> getVectorFromJson(const QByteArray & bytes);
    static JumpsImportancePreset getFromJson(QJsonObject json);
    bool saveToFile(QString dir);
    static JumpsImportancePreset loadFromJson(QString fileName);
};

#endif // JUMPSIMPORTANCEPRESET_H
