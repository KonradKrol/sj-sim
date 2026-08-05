#ifndef UUID_H
#define UUID_H

#include <QSet>
#include <functional>
#include "source/3rdparty/sole.hpp"

namespace sole
{
inline uint qHash(const uuid &value, uint seed = 0) noexcept
{
    return ::qHash(quint64(std::hash<uuid>{}(value)), seed);
}
}

class Uuid
{
public:
    static sole::uuid v4()
    {
        return sole::uuid4();
    }
};

#endif // UUID_H
