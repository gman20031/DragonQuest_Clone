#pragma once
// http://eiserloh.net/noise/SquirrelNoise5.hpp
// These noise functions were written by Squirrel Eiserloh as a cheap and simple substitute for
//	the [sometimes awful] bit-noise sample code functions commonly found on the web, many of which
//	are hugely biased or terribly patterned, e.g. having bits which are on (or off) 75% or even
//	100% of the time (or are excessively overkill/slow for our needs, such as MD5 or SHA).
#include <stdint.h>

namespace BlackBoxEngine
{
    namespace SquirrelNoise
    {
        //-----------------------------------------------------------------------------------------------
        // Raw pseudorandom noise functions (random-access / deterministic).  Basis of all other noise.
        //
        constexpr uint32_t Get1dNoiseUint( int32_t index, uint32_t seed = 0 );
        constexpr uint32_t Get2dNoiseUint( int32_t indexX, int32_t indexY, uint32_t seed = 0 );
        constexpr uint32_t Get3dNoiseUint( int32_t indexX, int32_t indexY, int32_t indexZ, uint32_t seed = 0 );
        constexpr uint32_t Get4dNoiseUint( int32_t indexX, int32_t indexY, int32_t indexZ, int32_t indexT, uint32_t seed = 0 );

        //-----------------------------------------------------------------------------------------------
        // Same functions, mapped to floats in [0,1] for convenience.
        //
        constexpr float Get1dNoiseZeroToOne( int32_t index, uint32_t seed = 0 );
        constexpr float Get2dNoiseZeroToOne( int32_t indexX, int32_t indexY, uint32_t seed = 0 );
        constexpr float Get3dNoiseZeroToOne( int32_t indexX, int32_t indexY, int32_t indexZ, uint32_t seed = 0 );
        constexpr float Get4dNoiseZeroToOne( int32_t indexX, int32_t indexY, int32_t indexZ, int32_t indexT, uint32_t seed = 0 );

        //-----------------------------------------------------------------------------------------------
        // Same functions, mapped to floats in [-1,1] for convenience.
        //
        constexpr float Get1dNoiseNegOneToOne( int32_t index, uint32_t seed = 0 );
        constexpr float Get2dNoiseNegOneToOne( int32_t indexX, int32_t indexY, uint32_t seed = 0 );
        constexpr float Get3dNoiseNegOneToOne( int32_t indexX, int32_t indexY, int32_t indexZ, uint32_t seed = 0 );
        constexpr float Get4dNoiseNegOneToOne( int32_t indexX, int32_t indexY, int32_t indexZ, int32_t indexT, uint32_t seed = 0 );

        constexpr uint32_t SquirrelNoise5( int32_t positionX, uint32_t seed )
        {
            constexpr uint32_t SQ5_BIT_NOISE1 = 0xd2a80a3f;	// 11010010101010000000101000111111
            constexpr uint32_t SQ5_BIT_NOISE2 = 0xa884f197;	// 10101000100001001111000110010111
            constexpr uint32_t SQ5_BIT_NOISE3 = 0x6C736F4B; // 01101100011100110110111101001011
            constexpr uint32_t SQ5_BIT_NOISE4 = 0xB79F3ABB;	// 10110111100111110011101010111011
            constexpr uint32_t SQ5_BIT_NOISE5 = 0x1b56c4f5;	// 00011011010101101100010011110101

            uint32_t mangledBits = (uint32_t)positionX;
            mangledBits *= SQ5_BIT_NOISE1;
            mangledBits += seed;
            mangledBits ^= (mangledBits >> 9);
            mangledBits += SQ5_BIT_NOISE2;
            mangledBits ^= (mangledBits >> 11);
            mangledBits *= SQ5_BIT_NOISE3;
            mangledBits ^= (mangledBits >> 13);
            mangledBits += SQ5_BIT_NOISE4;
            mangledBits ^= (mangledBits >> 15);
            mangledBits *= SQ5_BIT_NOISE5;
            mangledBits ^= (mangledBits >> 17);
            return mangledBits;
        }
        constexpr uint32_t Get1dNoiseUint( int32_t positionX, uint32_t seed )
        {
            return SquirrelNoise5( positionX, seed );
        }

        constexpr uint32_t Get2dNoiseUint( int32_t indexX, int32_t indexY, uint32_t seed )
        {
            constexpr int32_t PRIME_NUMBER = 198491317; // Large prime number with non-boring bits
            return SquirrelNoise5( indexX + (PRIME_NUMBER * indexY), seed );
        }

        constexpr uint32_t Get3dNoiseUint( int32_t indexX, int32_t indexY, int32_t indexZ, uint32_t seed )
        {
            constexpr int32_t PRIME1 = 198491317; // Large prime number with non-boring bits
            constexpr int32_t PRIME2 = 6542989; // Large prime number with distinct and non-boring bits
            return SquirrelNoise5( indexX + (PRIME1 * indexY) + (PRIME2 * indexZ), seed );
        }

        constexpr uint32_t Get4dNoiseUint( int32_t indexX, int32_t indexY, int32_t indexZ, int32_t indexT, uint32_t seed )
        {
            constexpr int32_t PRIME1 = 198491317; // Large prime number with non-boring bits
            constexpr int32_t PRIME2 = 6542989; // Large prime number with distinct and non-boring bits
            constexpr int32_t PRIME3 = 357239; // Large prime number with distinct and non-boring bits
            return SquirrelNoise5( indexX + (PRIME1 * indexY) + (PRIME2 * indexZ) + (PRIME3 * indexT), seed );
        }

        constexpr float Get1dNoiseZeroToOne( int32_t index, uint32_t seed )
        {
            constexpr double ONE_OVER_MAX_UINT = (1.0 / (double)0xFFFFFFFF);
            return (float)(ONE_OVER_MAX_UINT * (double)SquirrelNoise5( index, seed ));
        }

        constexpr float Get2dNoiseZeroToOne( int32_t indexX, int32_t indexY, uint32_t seed )
        {
            constexpr double ONE_OVER_MAX_UINT = (1.0 / (double)0xFFFFFFFF);
            return (float)(ONE_OVER_MAX_UINT * (double)Get2dNoiseUint( indexX, indexY, seed ));
        }

        constexpr float Get3dNoiseZeroToOne( int32_t indexX, int32_t indexY, int32_t indexZ, uint32_t seed )
        {
            constexpr double ONE_OVER_MAX_UINT = (1.0 / (double)0xFFFFFFFF);
            return (float)(ONE_OVER_MAX_UINT * (double)Get3dNoiseUint( indexX, indexY, indexZ, seed ));
        }

        constexpr float Get4dNoiseZeroToOne( int32_t indexX, int32_t indexY, int32_t indexZ, int32_t indexT, uint32_t seed )
        {
            constexpr double ONE_OVER_MAX_UINT = (1.0 / (double)0xFFFFFFFF);
            return (float)(ONE_OVER_MAX_UINT * (double)Get4dNoiseUint( indexX, indexY, indexZ, indexT, seed ));
        }

        constexpr float Get1dNoiseNegOneToOne( int32_t index, uint32_t seed )
        {
            constexpr double ONE_OVER_MAX_INT = (1.0 / (double)0x7FFFFFFF);
            return (float)(ONE_OVER_MAX_INT * (double)(int)SquirrelNoise5( index, seed ));
        }

        constexpr float Get2dNoiseNegOneToOne( int32_t indexX, int32_t indexY, uint32_t seed )
        {
            constexpr double ONE_OVER_MAX_INT = (1.0 / (double)0x7FFFFFFF);
            return (float)(ONE_OVER_MAX_INT * (double)(int)Get2dNoiseUint( indexX, indexY, seed ));
        }

        constexpr float Get3dNoiseNegOneToOne( int32_t indexX, int32_t indexY, int32_t indexZ, uint32_t seed )
        {
            constexpr double ONE_OVER_MAX_INT = (1.0 / (double)0x7FFFFFFF);
            return (float)(ONE_OVER_MAX_INT * (double)(int)Get3dNoiseUint( indexX, indexY, indexZ, seed ));
        }

        constexpr float Get4dNoiseNegOneToOne( int32_t indexX, int32_t indexY, int32_t indexZ, int32_t indexT, uint32_t seed )
        {
            constexpr double ONE_OVER_MAX_INT = (1.0 / (double)0x7FFFFFFF);
            return (float)(ONE_OVER_MAX_INT * (double)(int)Get4dNoiseUint( indexX, indexY, indexZ, indexT, seed ));
        }
    }
}