#include "SmoothStep.h"
#include <algorithm>

namespace BlackBoxEngine
{
    float smoothStep( float edge0, float edge1, float x )
    {
        x = std::clamp( (x - edge0) / (edge1 - edge0), 0.f, 1.f );
        return x * x * (3.0f - 2.0f * x);
    }

    float smootherStep( float edge0, float edge1, float x )
    {
        // Scale, and clamp x to 0..1 range
        x = std::clamp( (x - edge0) / (edge1 - edge0), 0.f, 1.f );
        return x * x * x * (x * (6.0f * x - 15.0f) + 10.0f);
    }
}