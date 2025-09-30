#include "RandomEngines.h"

namespace BlackBoxEngine
{
    namespace Random
    {
        namespace Engine
        {

            //////////////////////////////////////////////////////////////////////////////////////////
            /// SplitMix64
            //////////////////////////////////////////////////////////////////////////////////////////

            SplitMix64::SplitMix64()
            {
                Seed(kDefaultSeed);
            }

            SplitMix64::SplitMix64(uint64_t startingSeed)
            {
                Seed(startingSeed);
            }

            void SplitMix64::Seed(uint64_t Seed)
            {
                m_state = Seed;
            }

            uint64_t SplitMix64::Min()
            {
                return 0;
            }

            uint64_t SplitMix64::Max()
            {
                return std::numeric_limits<uint64_t>::max();
            }

            uint64_t SplitMix64::operator()()
            {
                //https://prng.di.unimi.it/xoshiro256plusplus.c -- recommended to convert to a 256 bit state
                uint64_t result = (m_state += 0x9E3779B97F4A7C15);
                result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
                result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
                return result ^ (result >> 31);
            }

            //////////////////////////////////////////////////////////////////////////////////////////
            /// Xoshiro256
            //////////////////////////////////////////////////////////////////////////////////////////

            uint64_t Xoshiro256::Roll(uint64_t value, uint64_t shift)
            {
                return (value << shift) | (value >> (64 - shift));
            }

            void Xoshiro256::FillState(uint64_t Seed)
            {
                SplitMix64 splitMix(Seed);
                m_state[0] = splitMix();
                m_state[1] = splitMix();
                m_state[2] = splitMix();
                m_state[3] = splitMix();
            }

            Xoshiro256::Xoshiro256()
            {
                Seed();
            }

            Xoshiro256::Xoshiro256(uint64_t firstSeed)
            {
                Seed(firstSeed);
            }

            void Xoshiro256::Seed()
            {
                FillState(kDefaultSeed);
            }

            void Xoshiro256::Seed(uint64_t newSeed)
            {
                FillState(newSeed);
            }

            uint64_t Xoshiro256::Min()
            {
                return 0;
            }

            uint64_t Xoshiro256::Max()
            {
                return std::numeric_limits<uint64_t>::max();
            }
           
            bool Xoshiro256::Bool()
            {
                uint64_t output = this->operator()();
                output = output >> 63;
                return output > 0;
            }

            uint64_t Xoshiro256::operator()()
            {
                uint64_t const result = Roll(m_state[0] + m_state[3], 23) + m_state[0];
                uint64_t const t = m_state[1] << 17;

                m_state[2] ^= m_state[0];
                m_state[3] ^= m_state[1];
                m_state[1] ^= m_state[2];
                m_state[0] ^= m_state[3];

                m_state[2] ^= t;
                m_state[3] = Roll(m_state[3], 45);

                return result;
            }

        } // engine

    } // random

} // Black box


