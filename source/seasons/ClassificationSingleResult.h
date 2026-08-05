#ifndef CLASSIFICATIONSINGLERESULT_H
#define CLASSIFICATIONSINGLERESULT_H

#include "../simulator/JumpData.h"
#include "../simulator/Jumper.h"
#include "../competitions/CompetitionResults.h"
#include "../utilities/Identifiable.h"
#include <QJsonObject>

class Classification;

class ClassificationSingleResult : public Identifiable
{
public:
    ClassificationSingleResult(Classification * classification, Jumper * jumper = nullptr);
    ClassificationSingleResult(Classification *classification, QString teamCode);

    void updateSingleResults();
    void addCompetitionResults(CompetitionResults & results);
    void updatePointsSum();

    static QJsonObject getJsonObject(ClassificationSingleResult & result);
    static ClassificationSingleResult getFromJson(QJsonObject json, IdentifiableObjectsStorage * storage);

private:
    Classification * classification;
    QVector<CompetitionResults *> competitionsResults;
    QVector<CompetitionSingleResult *> singleResults;

    Jumper * jumper;
    QString teamCode;

    double pointsSum;
    int position;

public:
    QVector<CompetitionResults *> getCompetitionsResults() const;
    QVector<CompetitionResults *> & getCompetitionsResultsReference();
    void setCompetitionsResults(const QVector<CompetitionResults *> &newCompetitionsResults);
    QVector<CompetitionSingleResult *> getSingleResults() const;
    QVector<CompetitionSingleResult *> & getSingleResultsReference();
    void setSingleResults(const QVector<CompetitionSingleResult *> &newSingleResults);
    Jumper *getJumper() const;
    void setJumper(Jumper *newJumper);
    QString getTeamCode() const;
    void setTeamCode(const QString &newTeamCode);
    double getPointsSum() const;
    void setPointsSum(double newPointsSum);
    int getPosition() const;
    void setPosition(int newPosition);
    Classification *getClassification() const;
    void setClassification(Classification *newClassification);
};

inline bool comparePointers(const ClassificationSingleResult * l, const ClassificationSingleResult * r)
{
    if(l->getPointsSum() == r->getPointsSum())
    {
        return l->getID().str() > r->getID().str();
    }
    else return l->getPointsSum() > r->getPointsSum();
}

inline bool operator>(const ClassificationSingleResult & l, const ClassificationSingleResult & r)
{
    if(l.getPointsSum() == r.getPointsSum())
    {
        return l.getID().str() > r.getID().str();
    }
    else return l.getPointsSum() > r.getPointsSum();
}

inline bool operator<(const ClassificationSingleResult & l, const ClassificationSingleResult & r)
{
    if(l.getPointsSum() == r.getPointsSum())
    {
        return l.getID() < r.getID();
    }
    else return l.getPointsSum() < r.getPointsSum();
}

#endif // CLASSIFICATIONSINGLERESULT_H
