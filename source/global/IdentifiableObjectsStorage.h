#ifndef IDENTIFIABLEOBJECTSSTORAGE_H
#define IDENTIFIABLEOBJECTSSTORAGE_H

#include <QMap>
#include "source/3rdparty/sole.hpp"
#include "source/utilities/Identifiable.h"

class IdentifiableObjectsStorage
{
public:
    void add(Identifiable* obj);

    template<typename T>
    void add(QVector<T*> & objects)
    {
        for(auto & obj : objects)
        {
            add(static_cast<Identifiable *>(obj));
        }
    }
    template<typename T>
    void add(QVector<T> & objects)
    {
        for(auto & obj : objects)
        {
            add(static_cast<Identifiable *>(&obj));
        }
    }

    void remove(sole::uuid uuid);
    Identifiable* get(sole::uuid uuid);
    Identifiable* get(QString stringUuid);
private:
    QMap<sole::uuid, Identifiable *> storage;
};

#endif // IDENTIFIABLEOBJECTSSTORAGE_H
