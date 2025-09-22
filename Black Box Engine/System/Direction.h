#pragma once

#include "../Math/FVector2.h"

namespace BlackBoxEngine
{
    enum class Direction
    {
        kUp,
        kDown,
        kLeft,
        kRight
    };

    namespace DirectionHelp
    {
        FVector2 DirectionToVector(Direction direction);

    }

}
