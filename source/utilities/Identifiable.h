#ifndef IDENTIFIABLE_H
#define IDENTIFIABLE_H

#include "../global/Uuid.h"

class Identifiable
{
public:
    Identifiable();

    bool operator==(const Identifiable & r) const;

    sole::uuid getID() const;
    QString getIDStr() const;
    void setID(sole::uuid id);

    void reassign();

protected:
    sole::uuid ID;
};

#endif // IDENTIFIABLE_H
