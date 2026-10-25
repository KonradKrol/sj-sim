#ifndef MYRANDOM_H
#define MYRANDOM_H

#include <QtGlobal>
#include <memory>


class MyRandom
{
public:
    MyRandom();
    class ScopedSeed {
        struct State;
        std::unique_ptr<State> previous;
    public:
        explicit ScopedSeed(quint64 seed);
        ~ScopedSeed();
        void reseed(quint64 seed);
        ScopedSeed(const ScopedSeed &)=delete;
        ScopedSeed &operator=(const ScopedSeed &)=delete;
    };

    static double randomDouble(double min, double max, short precision = 2);
    static int randomInt(int min, int max);
    static double reducingChancesRandom(double min, double max, double resultNumbersChange, double startingChance, double chancesChangeMultiplier, short drawType, short resultNumbersType);
    static double normalDistributionRandom(double base, double deviation);
    static double lognormalDistributionRandom(double base, double deviation);
    static double gammaDistributionRandom(double scale, double shape);
    static double normalDistributionRandomHalf(double base, double deviation, short half);

    enum DrawType{
        AddingChances, // dodawanie wszystkich szans i sprawdzanie po koleji czy random(0, sum) nie jest mniejszy od danej szansy, zaczynając od najmniejszych liczb;
        InTurnFromTheHighestChanceNumber, // czym wieksza liczba tym mniejsza sznasa
    };
    enum ResultNumbersType{
        FromSmallerToLarger,
        FromLargerToSmaller
    };
    enum NormalDistributionHalf{
        Positive,
        Negative
    };
};

#endif // MYRANDOM_H
