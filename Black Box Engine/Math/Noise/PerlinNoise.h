#pragma once

#include "NoiseGenerator.h"

namespace BlackBoxEngine
{
    class PerlinNoise : public NoiseGenerator
    {
    public:
        PerlinNoise( uint32_t seed, const char* pName ) : NoiseGenerator( seed, pName ) { /*EMPTY*/ };
        virtual float Noise2d( float x, float y, uint32_t seedOverride ) const override;
    private:
        float CaluclateDotProductOfGridGradient(
            float perlinGridX, float perlinGridY,
            float  x, float y, uint32_t seedOverride ) const;
    };

}