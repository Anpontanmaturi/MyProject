#pragma once

#include "blackboard_value.h"
#include "blackboard.h"

namespace BlackboardKeys
{
    inline const BlackboardKey PlayerPosition
    {
        "PlayerPosition",
        BlackboardValueType::Float3
    };

    inline const BlackboardKey Player
    {
        "Character::Player",
        BlackboardValueType::Character
    };

    inline const BlackboardKey Sandbag
    {
        "Character::Sandbag",
        BlackboardValueType::Character
    };
}