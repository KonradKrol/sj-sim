#include "Identifiable.h"
#include <QDebug>

Identifiable::Identifiable()
{
    ID = Uuid::v4();
}

bool Identifiable::operator==(const Identifiable &r) const
{
    return this->getID() == r.getID();
}

sole::uuid Identifiable::getID() const
{
    return ID;
}

QString Identifiable::getIDStr() const
{
    return QString::fromStdString(ID.str());
}

void Identifiable::setID(sole::uuid id)
{
    this->ID = id;
}

void Identifiable::reassign()
{
    ID = Uuid::v4();
}
