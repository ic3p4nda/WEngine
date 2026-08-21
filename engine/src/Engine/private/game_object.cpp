#include "game_object.h"

namespace ZEngine
{
    ZGameObject ZGameObject::createGameObject()
    {
        static id_t currentId = 0;
        return ZGameObject{ currentId++ };
    }
}
