#pragma once

#include <string>

enum class BlackboardValueType
{
    Int,
    Float,
    Bool,
    String,
    Character,
    Float3
};

struct BlackboardKey
{
    std::string name;
    BlackboardValueType type;
};


namespace BlackboardKeys
{
    inline const BlackboardKey PlayerPosition
    {
        "PlayerPosition",
        BlackboardValueType::Float3
    };

    inline const BlackboardKey Player
    {
        "Player",
        BlackboardValueType::Character
    };

    inline const BlackboardKey Enemy
    {
        "Enemy",
        BlackboardValueType::Character
    };
}