#include "BlackBoxGame.h"

#include <Math/Random.h>

int main([[maybe_unused]]int argc, [[maybe_unused]] char** argv)
{
    BlackBoxGame::NewSingleton()->Launch();
}
