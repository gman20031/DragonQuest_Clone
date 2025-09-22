#include "Direction.h"

namespace BlackBoxEngine
{
    namespace DirectionHelp
    {

        FVector2 DirectionToVector(Direction direction)
        {
            const FVector2 s_vectors[] = {
                { 0, -1},
                { 0, +1},
                { -1, 0},
                { +1, 0}
            };
            return s_vectors[static_cast<uint32_t>(direction)];
        }
    }
}
