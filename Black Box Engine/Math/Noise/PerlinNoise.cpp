#include "PerlinNoise.h"

#include <math.h>
#include <cmath>
#include <algorithm>

#include "SquirrelNoise5.h"
#include "SmoothStep.h"

namespace BlackBoxEngine
{
    static constexpr bool kUseSmootherStep = false;

    float PerlinNoise::CaluclateDotProductOfGridGradient(
        float perlinGridX, float perlinGridY, float x, float y, uint32_t seedOverride ) const
    {
        float gradX = SquirrelNoise::Get2dNoiseNegOneToOne( (int32_t)perlinGridX, (int32_t)perlinGridY, seedOverride );
        float gradY = SquirrelNoise::Get2dNoiseNegOneToOne( (int32_t)perlinGridX, (int32_t)perlinGridY, SquirrelNoise::Get1dNoiseUint( seedOverride ) );

        float len = sqrt( (gradX * gradX) + (gradY * gradY) );
        gradX /= len;
        gradY /= len;

        // vector from gridX, gridY to x,y
        float dx = x - perlinGridX;
        float dy = y - perlinGridY;

        return ((dx * gradX) + (dy * gradY));
    }

    float PerlinNoise::Noise2d( float x, float y, uint32_t seedOverride ) const
    {
        // this is mostly just from the demo

        float x0 = floorf( x );
        float y0 = floorf( y );
        float x1 = x0 + 1;
        float y1 = y0 + 1;

        // Calculate the delta position (i.e. how far away from the top left grid point 
        // this point is) and smooth the results.  These will be used as lerp weights.
        float smoothWeightX = 0.f;
        float smoothWeightY = 0.f;
        if constexpr ( kUseSmootherStep )
        {
            smoothWeightX = smootherStep( 0, 1, x - x0 );
            smoothWeightY = smootherStep( 0, 1, y - y0 );
        }
        else
        {
            smoothWeightX = smoothStep( 0, 1, x - x0 );
            smoothWeightY = smoothStep( 0, 1, y - y0 );
        }

        // interpolate between grid point gradients
        // top two vectors
        float range0 = CaluclateDotProductOfGridGradient( x0, y0, x, y, seedOverride );
        float range1 = CaluclateDotProductOfGridGradient( x1, y0, x, y, seedOverride );
        float blendX0 = std::lerp( range0, range1, smoothWeightX );

        // bottom two vectors
        range0 = CaluclateDotProductOfGridGradient( x0, y1, x, y, seedOverride );
        range1 = CaluclateDotProductOfGridGradient( x1, y1, x, y, seedOverride );
        float blendX1 = std::lerp( range0, range1, smoothWeightX );

        float retNoise = std::lerp( blendX0, blendX1, smoothWeightY );
        return retNoise;
    }
}