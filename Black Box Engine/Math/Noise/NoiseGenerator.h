#pragma once

#include <stdint.h>

namespace BlackBoxEngine
{
    class NoiseGenerator
    {
    protected:
        uint32_t m_seed;

        const char* m_pName = "Noise Generator";
        float m_worldWidth = 0;
        float m_worldHeight = 0;
        int m_noiseRange = 0;
        int m_octaves = 0;
        float m_persistance = 0;
    public:
        NoiseGenerator( uint32_t seed, const char* pName ) : m_seed( seed ), m_pName( pName ) {};
        virtual ~NoiseGenerator() = default;

        virtual float Noise2d( float x, float y ) { return Noise2d( x, y, m_seed ); }
        virtual float Noise2d( float x, float y, uint32_t seedOverride ) const = 0;

        virtual float NoiseWithOctavesWorld(
            float x,
            float y,
            float worldWidth,
            float worldHeight,
            float noiseInputRange,
            int numOctaves,
            float persistance
        ) const;

        virtual float NoiseWorldWithPresets(
            float x,
            float y
        ) const;
        void PrintInfo() const;

        uint32_t GetSeed() const { return m_seed; }

        virtual void Reseed( uint32_t seed );
        virtual void ChangeOctaves( int amount );
        virtual void ChangeNoiseRange( int amount );
        virtual void ChangePersistence( float amount );

        virtual void SetOctaves( int amount );
        virtual void SetNoiseRange( int amount );
        virtual void SetPersistence( float amount );
        virtual void SetWorldSize( float x, float y );
    private:
        virtual float NoiseWorld2d(
            float x, float y, float worldWidth, float worldHeight, float noiseInputRange, uint32_t seed ) const;

    };

}

