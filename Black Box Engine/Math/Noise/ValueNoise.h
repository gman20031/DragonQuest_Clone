#pragma once

#include "NoiseGenerator.h"

namespace BlackBoxEngine
{
    class ValueNoise : public NoiseGenerator
    {
    public:
        ValueNoise( uint32_t seed, const char* pName ) : NoiseGenerator( seed, pName ) { /*EMPTY*/ };

        float Noise2d( float x, float y, uint32_t optionalSeed ) const;
    };
}