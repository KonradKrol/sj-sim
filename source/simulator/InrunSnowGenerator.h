#ifndef INRUNSNOWGENERATOR_H
#define INRUNSNOWGENERATOR_H


class InrunSnowGenerator
{
public:
    InrunSnowGenerator(double baseInrunSnow = 0, double inrunSnowDeviation = 0);

private:
    double baseInrunSnow;
    double inrunSnowDeviation;
public:
    double generateInrunSnow();
public:
    double getBaseInrunSnow() const;
    void setBaseInrunSnow(double newBaseInrunSnow);
    double getInrunSnowDeviation() const;
    void setInrunSnowDeviation(double newInrunSnowDeviation);
};

#endif // INRUNSNOWGENERATOR_H
