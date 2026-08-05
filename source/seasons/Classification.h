#ifndef ABSTRACTCLASSIFICATION_H
#define ABSTRACTCLASSIFICATION_H

#include <QMap>
#include <QString>
#include <QJsonObject>
#include "../utilities/Identifiable.h"
#include "ClassificationSingleResult.h"
#include "../seasons/SeasonCalendar.h"
#include <dpp/dpp.h>

class Season;
class SimulationSave;

class Classification : public Identifiable
{
public:
    Classification(QString name = "");
    ~Classification();

    enum PunctationType{
        PointsForPlaces,
        CompetitionPoints
    };

    enum ClassificationType{
        Individual,
        Team
    };

    void updatePositions();
    void sortInDescendingOrder();
    void sortInAscendingOrder();

    ClassificationSingleResult * getResultOfIndividualJumper(Jumper * jumper);
    ClassificationSingleResult * getResultOfTeam(QString teamCode);
    ClassificationSingleResult * getResultByIndex(int index);

    static QVector<Classification *> getSpecificTypeClassifications(QVector<Classification *> classifications, int type);

    static Classification *getFromJson(QJsonObject obj, IdentifiableObjectsStorage * storage, int * oldId = nullptr);
    static QJsonObject getJsonObject(Classification *classification);

    QHash<Jumper *, QHash<CompetitionInfo *, int> > constructJumpersArchiveResults(Season * season, SeasonCalendar *calendar);
    QHash<QString, QHash<CompetitionInfo *, int> > constructTeamsArchiveResults(Season * season, SeasonCalendar *calendar);

    QString getSingleResultsTextForWebhook();
    dpp::message getMessageForResultsWebhook(SimulationSave *save);
    void sendResultsWebhook(SimulationSave *save);

private:
    QString name;
    short classificationType;
    short punctationType;
    QMap<int, double> pointsForPlaces;
    QMap<int, double> altPointsForPlaces;

    QVector<ClassificationSingleResult *> results;

public:
    short getClassificationType() const;
    void setClassificationType(short newClassificationType);
    QMap<int, double> getPointsForPlaces() const;
    QMap<int, double> &getPointsForPlacesReference();
    void setPointsForPlaces(const QMap<int, double> &newPointsForPlaces);
    QString getName() const;
    void setName(const QString &newName);
    short getPunctationType() const;
    void setPunctationType(short newPunctationType);
    QVector<ClassificationSingleResult *> getResults() const;
    QVector<ClassificationSingleResult *> & getResultsReference();
    void setResults(const QVector<ClassificationSingleResult *> &newResults);
    QMap<int, double> getAltPointsForPlaces() const;
    QMap<int, double> & getAltPointsForPlacesReference();
    void setAltPointsForPlaces(const QMap<int, double> &newAltPointsForPlaces);
};

#endif // ABSTRACTCLASSIFICATION_H
