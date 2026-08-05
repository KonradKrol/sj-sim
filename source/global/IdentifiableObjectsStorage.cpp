#include "IdentifiableObjectsStorage.h"

void IdentifiableObjectsStorage::add(Identifiable *obj)
{
    storage.insert(obj->getID(), obj);
}

void IdentifiableObjectsStorage::remove(sole::uuid uuid)
{
    if(storage.contains(uuid))
        storage.remove(uuid);
}

Identifiable *IdentifiableObjectsStorage::get(sole::uuid uuid)
{
    if(storage.contains(uuid))
        return storage.value(uuid);
    else
        return nullptr;
}

Identifiable *IdentifiableObjectsStorage::get(QString stringUuid)
{
    return get(sole::rebuild(stringUuid.toStdString()));
}
