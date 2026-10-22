#ifndef UUID_H
#define UUID_H

#include <QSet>
#include <functional>
#include "source/3rdparty/sole.hpp"

namespace sole
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
inline size_t qHash(const uuid &value, size_t seed = 0) noexcept
#else
inline uint qHash(const uuid &value, uint seed = 0) noexcept
#endif
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
