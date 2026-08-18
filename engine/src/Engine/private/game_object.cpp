#include "game_object.h"

namespace Engine
{
    GameObject GameObject::createGameObject()
    {
        static id_t currentId = 0;
        return GameObject{ currentId++ };
    }
}
