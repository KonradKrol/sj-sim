#ifndef JUMPDATAINFOCHOICE_H
#define JUMPDATAINFOCHOICE_H

#include <QJsonObject>

class JumpDataInfoChoice
{
public:
    JumpDataInfoChoice();

    static QJsonObject getJsonObject(const JumpDataInfoChoice & choice);
    static JumpDataInfoChoice getFromJson(QJsonObject obj);

private:
    bool distance;
    bool points;
    bool gate;
    bool gateCompensation;
    bool windCompensation;
    bool totalCompensation;
    bool judges;
    bool judgesPoints;
    bool averagedWind;
    bool specificWind;
    bool jumpPositionInRound;
    bool takeoffRating;
    bool flightRating;
    bool distanceRandom;
    bool inrunSnow;
    bool landingType;
    bool positionAfterJump;
public:
    bool getDistance() const;
    void setDistance(bool newDistance);
    bool getPoints() const;
    void setPoints(bool newPoints);
    bool getGate() const;
    void setGate(bool newGate);
    bool getGateCompensation() const;
    void setGateCompensation(bool newGateCompensation);
    bool getWindCompensation() const;
    void setWindCompensation(bool newWindCompensation);
    bool getTotalCompensation() const;
    void setTotalCompensation(bool newTotalCompensation);
    bool getJudges() const;
    void setJudges(bool newJudges);
    bool getJudgesPoints() const;
    void setJudgesPoints(bool newJudgesPoints);
    bool getAveragedWind() const;
    void setAveragedWind(bool newAveragedWind);
    bool getSpecificWind() const;
    void setSpecificWind(bool newSpecificWind);
    bool getJumpPositionInRound() const;
    void setJumpPositionInRound(bool newJumpPositionInRound);
    bool getTakeoffRating() const;
    void setTakeoffRating(bool newTakeoffRating);
    bool getFlightRating() const;
    void setFlightRating(bool newFlightRating);
    bool getInrunSnow() const;
    void setInrunSnow(bool newInrunSnow);
    bool getLandingType() const;
    void setLandingType(bool newLandingType);
    bool getPositionAfterJump() const;
    void setPositionAfterJump(bool newPositionAfterJump);
    bool getDistanceRandom() const;
    void setDistanceRandom(bool newDistanceRandom);
};

#endif // JUMPDATAINFOCHOICE_H
