#include "ValueNoise.h"

#include <math.h>
#include <cmath>
#include <algorithm>

#include "SquirrelNoise5.h"
#include "SmoothStep.h"


namespace BlackBoxEngine
{
    static constexpr bool kUseSmootherStep = false;

    float ValueNoise::Noise2d( float x, float y, uint32_t seed ) const
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

        float topLeftNoise = SquirrelNoise::Get2dNoiseNegOneToOne( static_cast<int32_t>(x0), static_cast<int32_t>(y0), seed );
        float topRightNoise = SquirrelNoise::Get2dNoiseNegOneToOne( static_cast<int32_t>(x1), static_cast<int32_t>(y0), seed );
        float bottomLeftNoise = SquirrelNoise::Get2dNoiseNegOneToOne( static_cast<int32_t>(x0), static_cast<int32_t>(y1), seed );
        float bottomRightNoise = SquirrelNoise::Get2dNoiseNegOneToOne( static_cast<int32_t>(x1), static_cast<int32_t>(y1), seed );

        // interpolate the noise, using the smoothed X value as a weight
        float resultX = std::lerp( topLeftNoise, topRightNoise, smoothWeightX );
        float resultY = std::lerp( bottomLeftNoise, bottomRightNoise, smoothWeightX );  // Note: smoothWeightX, not smoothWeightY

        // Finally, interpolate between resulting x and y values using the 
        // smoothed y value as a weight.  This is our final noise value.
        float finalNoise = std::lerp( resultX, resultY, smoothWeightY );
        return finalNoise;
    }
}