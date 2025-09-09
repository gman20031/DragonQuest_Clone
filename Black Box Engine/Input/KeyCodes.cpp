#include "KeyCodes.h"

#include <SDL.h>

std::ostream& BlackBoxEngine::operator<<(std::ostream& os, KeyCode out)
{
    os << (size_t)out;
    return os;
}
