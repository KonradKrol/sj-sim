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
    bool distance = false;
    bool points = false;
    bool gate = false;
    bool gateCompensation = false;
    bool windCompensation = false;
    bool totalCompensation = false;
    bool judges = false;
    bool judgesPoints = false;
    bool averagedWind = false;
    bool specificWind = false;
    bool jumpPositionInRound = false;
    bool takeoffRating = false;
    bool flightRating = false;
    bool distanceRandom = false;
    bool inrunSnow = false;
    bool landingType = false;
    bool positionAfterJump = false;
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
