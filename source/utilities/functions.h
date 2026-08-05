#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QVector>
#include <QWidget>
#include <vector>
#include <QRegularExpression>
#include <QMessageLogContext>
#include <QString>
#include <QMessageLogContext>
#include <QtGlobal>
#include <QtMath>
#include <QVarLengthArray>
#include "../competitions/CompetitionInfo.h"

template<typename T>
void moveItemToVectorBack(QVector<T> & vector, size_t itemIndex)
{
    auto it = vector.begin() + itemIndex;
    std::rotate(it, it+1, vector.end());
}

class MyFunctions{
public:
    static QVector<QWidget *> getWidgetsVector(QWidget *parent, QString search);

    template<typename T>
    static bool vectorContains(const QVector<T *> & vector, T * item)
    {
        for(auto & it : vector){
            if(it == item)
                return true;
        }
        return false;
    }
    template<typename T>
    static bool vectorContains(const QVector<T> & vector, T * item)
    {
        for(auto & it : vector){
            if(&it == item)
                return true;
        }
        return false;
    }
    template<typename T>
    static bool vectorContainsByID(const QVector<T> & vector, T * item)
    {
        for(auto & it : vector){
            if(it.getID() == item->getID())
                return true;
        }
        return false;
    }
    template<typename T>
    static bool vectorContainsByID(const QVector<T *> & vector, T * item)
    {
        for(auto & it : vector){
            if(it->getID() == item->getID())
                return true;
        }
        return false;
    }

    template<typename T>
    static T * getVectorItemByID(QVector<T> & vector, sole::uuid ID)
    {
        for(auto & it : vector){
            if(it.getID() == ID)
                return &it;
        }
        return nullptr;
    }

    template<typename T>
    static void removeFromVector(QVector<T*> & vector, T * item)
    {
        int i=0;
        for(auto & it : vector)
        {
            if(it == item){
                vector.remove(i);
                break;
            }
            i++;
        }
    }

    template<typename T>
    static void removeFromVector(QVector<T> & vector, T * item)
    {
        int i=0;
        for(auto & it : vector)
        {
            if(&it == item){
                vector.remove(i);
                break;
            }
            i++;
        }
    }

    template<typename T>
    static void removeFromVectorByID(QVector<T> & vector, sole::uuid id)
    {
        int i=0;
        for(auto & it : vector)
        {
            if(it->getID() == id){
                vector.remove(i);
                break;
            }
            i++;
        }
    }

    template<typename T>
    static QVector<T *> convertToVectorOfPointers(QVector<T> * vector)
    {
        QVector<T *> toReturn;
        for(auto & obj : *vector)
        {
            toReturn.push_back(&obj);
        }
        return toReturn;
    }

    template<typename T>
    static int getIndexOfItemInVector(QVector<T> & vector, T * item)
    {
        int index=0;
        for(auto & el : vector)
        {
            if(&el == item)
            {
                return index;
            }
            index++;
        }
        return -1;
    }
    template<typename T>
    static int getIndexOfItemInVector(QVector<T*> & vector, T * item)
    {
        int index=0;
        for(auto & el : vector)
        {
            if(el == item)
            {
                return index;
            }
            index++;
        }
        return -1;
    }
    template<typename T>
    static int getIndexOfItemInVectorById(QVector<T> & vector, T * item)
    {
        int index=0;
        for(auto & el : vector)
        {
            if(el.getID() == item->getID())
            {
                return index;
            }
            index++;
        }
        return -1;
    }

    /*template<typename T, typename AverageType>
    static T standardDeviation(QVector<T> & values, AverageType average)
    {
        T dev = 0;
        for(auto & value : values)
        {
            dev += qPow(qAbs(value - average), 2);
        }
        dev = qSqrt(dev / (values.count() - 1));
    }*/

    static double standardDeviation(QVector<int> & values, double average)
    {
        double dev = 0;
        for(auto & value : values)
        {
            dev += qPow(qAbs(double(value) - average), 2);
        }
        dev = qSqrt(dev / (values.count() - 1));
        return dev;
    }

    static void fileMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg);
};

double roundDoubleToHalf(double number);
double roundDoubleToOnePlace(double number);
double roundDoubleToTwoPlaces(double number);


#endif // FUNCTIONS_H
