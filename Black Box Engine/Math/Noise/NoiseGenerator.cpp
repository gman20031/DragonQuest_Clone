#include "NoiseGenerator.h"

#include "../../System/log.h"
#include "SquirrelNoise5.h"

namespace BlackBoxEngine
{
    float NoiseGenerator::NoiseWithOctavesWorld(
        float x,
        float y,
        float worldWidth,
        float worldHeight,
        float noiseInputRange,
        int numOctaves,
        float persistance ) const
    {
        if ( numOctaves <= 0 )
            return -1;

        float noise = 0;
        float currentAplitude = 1;
        float totalAmplitude = 0;
        for ( int i = numOctaves; i > 0; --i )
        {
            totalAmplitude += currentAplitude;

            uint32_t seed = SquirrelNoise::Get1dNoiseUint( (numOctaves + 1) - i, m_seed );
            float localNoise = NoiseWorld2d( x, y, worldWidth, worldHeight, noiseInputRange, seed );
            noise += localNoise * currentAplitude;
            currentAplitude *= persistance;
            noiseInputRange *= 2;
        }
        noise /= totalAmplitude;
        return noise;
    }

    float NoiseGenerator::NoiseWorldWithPresets( float x, float y ) const
    {
        return NoiseWithOctavesWorld( x, y, m_worldWidth, m_worldHeight, (float)m_noiseRange, m_octaves, m_persistance );
    }

    void NoiseGenerator::PrintInfo() const
    {
        BB_LOG(LogType::kMessage, m_pName       , '\n'
            , "World width  > ", m_worldWidth   , '\n'
            , "World Height > " , m_worldHeight , '\n'
            , "Noise Range  > " , m_noiseRange  , '\n'
            , "Octaves      > " , m_octaves     , '\n'
            , "Persistance  > " , m_persistance);
    }

    float NoiseGenerator::NoiseWorld2d(
        float x, float y, float worldWidth, float worldHeight, float noiseInputRange, uint32_t seed ) const
    {
        float noiseGridX = (x / worldWidth) * noiseInputRange;
        float noiseGridY = (y / worldHeight) * noiseInputRange;
        return Noise2d( noiseGridX, noiseGridY, seed );
    }

    void NoiseGenerator::Reseed( uint32_t seed )
    {
        m_seed = seed;
        BB_LOG( LogType::kMessage, "seed is now > ", seed );
    }

    void NoiseGenerator::ChangeOctaves( int amount )
    {
        m_octaves += amount;
        if ( m_octaves <= 0 )
            m_octaves = 1;
        BB_LOG(LogType::kMessage,  m_pName , "'s Octaves is now > " , m_octaves);
    }

    void NoiseGenerator::ChangeNoiseRange( int amount )
    {
        m_noiseRange += amount;
        if ( m_noiseRange <= 0 )
            m_noiseRange = 1;
        BB_LOG(LogType::kMessage,  m_pName , "'s Noise Input Range is now > " , m_noiseRange);
    }

    void NoiseGenerator::ChangePersistence( float amount )
    {
        m_persistance += amount;
        BB_LOG(LogType::kMessage,  m_pName , "'s Persistence is now > " , m_persistance);
    }

    void NoiseGenerator::SetOctaves( int amount )
    {
        m_octaves = amount;
        if ( m_octaves <= 0 )
            m_octaves = 1;
        BB_LOG(LogType::kMessage,  m_pName , "'s Octaves is now > " , m_octaves);
    }

    void NoiseGenerator::SetNoiseRange( int amount )
    {
        m_noiseRange = amount;
        if ( m_noiseRange <= 0 )
            m_noiseRange = 1;
        BB_LOG(LogType::kMessage,  m_pName , "'s Noise Input Range is now > " , m_noiseRange);
    }

    void NoiseGenerator::SetPersistence( float amount )
    {
        m_persistance = amount;
        BB_LOG(LogType::kMessage,  m_pName , "'s Persistence is now > " , m_persistance);
    }

    void NoiseGenerator::SetWorldSize( float x, float y )
    {
        m_worldWidth = x;
        m_worldHeight = y;
    }

}