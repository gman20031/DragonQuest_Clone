#pragma once

#include <stdint.h>
#include <concepts>

#include "../System/Log.h"

namespace BlackBoxEngine
{
    namespace Random
    {
        /**
         * @brief Concept to restrain the use of engines for prng machine without the need for inheritance. 
         * Name structure follows the STL for interchangability.
         * @param Defined a result_type which is an unsigned integer
         * @param Operator() which returns the random number, of result_type
         * @param Seed(void) function
         * @param Seed(result_type) which will Seed the engine
         * @param Discard(result_type) which will Discard calls to the engine x number of times
         */
        template<typename engineType>
        concept PRNG_Engine =
            std::unsigned_integral<typename engineType::result_type> &&
            requires(engineType engine, engineType::result_type integer)
        {
            { engine() } -> std::same_as<typename engineType::result_type>;
            { engine.Seed() };
            { engine.Seed(integer) };
            { engine.Max() } -> std::same_as<typename engineType::result_type>;
            { engine.Min() } -> std::same_as<typename engineType::result_type>;
        };

        template<typename engineType>
        concept BoolSpecialized = requires(engineType engine)
        {
            { engine.Bool() } -> std::same_as<bool>;
        };

        namespace Engine
        {

            //////////////////////////////////////////////////////////////////////////////////////////
            /// SplitMix64
            //////////////////////////////////////////////////////////////////////////////////////////
            
            class SplitMix64
            {
            public:
                using result_type = uint64_t;
                static constexpr uint64_t kDefaultSeed = 0xABCDEF12; // Not sure if this should be some specific value.
            private:
                uint64_t m_state;
            public:
                SplitMix64();
                SplitMix64(uint64_t startingSeed);
                void Seed();
                void Seed(uint64_t Seed);
                uint64_t Min();
                uint64_t Max();
                uint64_t operator()();
            };

            //////////////////////////////////////////////////////////////////////////////////////////
            /// Xoshiro256
            //////////////////////////////////////////////////////////////////////////////////////////

            class Xoshiro256
            {
            public:
                using result_type = uint64_t;
                static constexpr uint64_t kDefaultSeed = 0xABCDEF12; // Not sure if this should be some specific value.
            private:
                uint64_t m_state[4];

            private:
                uint64_t Roll(uint64_t value, uint64_t shift);
                void FillState(uint64_t Seed);

            public:
                Xoshiro256();
                Xoshiro256(uint64_t firstSeed);

                void Seed();
                void Seed(uint64_t Seed);
                bool Bool();
                uint64_t Min();
                uint64_t Max();
                uint64_t operator()();
            };

        }

    }

}

