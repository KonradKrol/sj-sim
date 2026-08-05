#include "JumpDataInfoChoice.h"

JumpDataInfoChoice::JumpDataInfoChoice()
{

}

QJsonObject JumpDataInfoChoice::getJsonObject(const JumpDataInfoChoice &choice)
{
    QJsonObject object;
    object.insert("gate-compensation", choice.getGateCompensation());
    object.insert("distance", choice.getDistance());
    object.insert("points", choice.getPoints());
    object.insert("gate", choice.getGate());
    object.insert("averaged-wind", choice.getAveragedWind());
    object.insert("wind-compensation", choice.getWindCompensation());
    object.insert("total-compensation", choice.getTotalCompensation());
    object.insert("judges", choice.getJudges());
    object.insert("judges-points", choice.getJudgesPoints());
    object.insert("landing-type", choice.getLandingType());
    object.insert("position-after-jump", choice.getPositionAfterJump());
    object.insert("jump-position-in-round", choice.getJumpPositionInRound());
    object.insert("specific-wind", choice.getSpecificWind());
    object.insert("takeoff-rating", choice.getTakeoffRating());
    object.insert("flight-rating", choice.getFlightRating());
    object.insert("distance-random", choice.getDistanceRandom());
    object.insert("inrun-snow", choice.getInrunSnow());
    return object;
}

JumpDataInfoChoice JumpDataInfoChoice::getFromJson(QJsonObject obj)
{
    JumpDataInfoChoice choice;
    choice.setGateCompensation(obj.value("gate-compensation").toBool(false));
    choice.setDistance(obj.value("distance").toBool(false));
    choice.setPoints(obj.value("points").toBool(false));
    choice.setGate(obj.value("gate").toBool(false));
    choice.setAveragedWind(obj.value("averaged-wind").toBool(false));
    choice.setWindCompensation(obj.value("wind-compensation").toBool(false));
    choice.setTotalCompensation(obj.value("total-compensation").toBool(false));
    choice.setJudges(obj.value("judges").toBool(false));
    choice.setJudgesPoints(obj.value("judges-points").toBool(false));
    choice.setLandingType(obj.value("landing-type").toBool(false));
    choice.setPositionAfterJump(obj.value("position-after-jump").toBool(false));
    choice.setJumpPositionInRound(obj.value("jump-position-in-round").toBool(false));
    choice.setSpecificWind(obj.value("specific-wind").toBool(false));
    choice.setTakeoffRating(obj.value("takeoff-rating").toBool(false));
    choice.setFlightRating(obj.value("flight-rating").toBool(false));
    choice.setDistanceRandom(obj.value("distance-random").toBool(false));
    choice.setInrunSnow(obj.value("inrun-snow").toBool(false));
    return choice;
}

bool JumpDataInfoChoice::getDistanceRandom() const
{
    return distanceRandom;
}

void JumpDataInfoChoice::setDistanceRandom(bool newDistanceRandom)
{
    distanceRandom = newDistanceRandom;
}

bool JumpDataInfoChoice::getDistance() const
{
    return distance;
}

void JumpDataInfoChoice::setDistance(bool newDistance)
{
    distance = newDistance;
}

bool JumpDataInfoChoice::getPoints() const
{
    return points;
}

void JumpDataInfoChoice::setPoints(bool newPoints)
{
    points = newPoints;
}

bool JumpDataInfoChoice::getGate() const
{
    return gate;
}

void JumpDataInfoChoice::setGate(bool newGate)
{
    gate = newGate;
}

bool JumpDataInfoChoice::getGateCompensation() const
{
    return gateCompensation;
}

void JumpDataInfoChoice::setGateCompensation(bool newGateCompensation)
{
    gateCompensation = newGateCompensation;
}

bool JumpDataInfoChoice::getWindCompensation() const
{
    return windCompensation;
}

void JumpDataInfoChoice::setWindCompensation(bool newWindCompensation)
{
    windCompensation = newWindCompensation;
}

bool JumpDataInfoChoice::getTotalCompensation() const
{
    return totalCompensation;
}

void JumpDataInfoChoice::setTotalCompensation(bool newTotalCompensation)
{
    totalCompensation = newTotalCompensation;
}

bool JumpDataInfoChoice::getJudges() const
{
    return judges;
}

void JumpDataInfoChoice::setJudges(bool newJudges)
{
    judges = newJudges;
}

bool JumpDataInfoChoice::getJudgesPoints() const
{
    return judgesPoints;
}

void JumpDataInfoChoice::setJudgesPoints(bool newJudgesPoints)
{
    judgesPoints = newJudgesPoints;
}

bool JumpDataInfoChoice::getAveragedWind() const
{
    return averagedWind;
}

void JumpDataInfoChoice::setAveragedWind(bool newAveragedWind)
{
    averagedWind = newAveragedWind;
}

bool JumpDataInfoChoice::getSpecificWind() const
{
    return specificWind;
}

void JumpDataInfoChoice::setSpecificWind(bool newSpecificWind)
{
    specificWind = newSpecificWind;
}

bool JumpDataInfoChoice::getJumpPositionInRound() const
{
    return jumpPositionInRound;
}

void JumpDataInfoChoice::setJumpPositionInRound(bool newJumpPositionInRound)
{
    jumpPositionInRound = newJumpPositionInRound;
}

bool JumpDataInfoChoice::getTakeoffRating() const
{
    return takeoffRating;
}

void JumpDataInfoChoice::setTakeoffRating(bool newTakeoffRating)
{
    takeoffRating = newTakeoffRating;
}

bool JumpDataInfoChoice::getFlightRating() const
{
    return flightRating;
}

void JumpDataInfoChoice::setFlightRating(bool newFlightRating)
{
    flightRating = newFlightRating;
}

bool JumpDataInfoChoice::getInrunSnow() const
{
    return inrunSnow;
}

void JumpDataInfoChoice::setInrunSnow(bool newInrunSnow)
{
    inrunSnow = newInrunSnow;
}

bool JumpDataInfoChoice::getLandingType() const
{
    return landingType;
}

void JumpDataInfoChoice::setLandingType(bool newLandingType)
{
    landingType = newLandingType;
}

bool JumpDataInfoChoice::getPositionAfterJump() const
{
    return positionAfterJump;
}

void JumpDataInfoChoice::setPositionAfterJump(bool newPositionAfterJump)
{
    positionAfterJump = newPositionAfterJump;
}
