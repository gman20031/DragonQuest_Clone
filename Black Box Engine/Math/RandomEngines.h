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
         * @brief https://en.cppreference.com/w/cpp/named_req/RandomNumberEngine.html
         * @param Defined a result_type which is an unsigned integer
         * @param Operator() which returns the random number, of result_type
         * @param seed(void) function
         * @param seed(result_type) which will seed the engine
         * @param discard(result_type) which will discard calls to the engine x number of times
         */
        template<typename engineType>
        concept PRNG_Engine =
            std::unsigned_integral<typename engineType::result_type> &&
            requires(engineType engine, engineType::result_type integer)
        {
            { engine() } -> std::same_as<typename engineType::result_type>;
            { engine.seed() };
            { engine.seed(integer) };
            { engine.discard(integer) };
            { engine.max() } -> std::same_as<typename engineType::result_type>;
            { engine.min() } -> std::same_as<typename engineType::result_type>;
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
                void seed();
                void seed(uint64_t seed);
                void discard(uint64_t amount);
                uint64_t min();
                uint64_t max();
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
                void FillState(uint64_t seed);

            public:
                Xoshiro256();
                Xoshiro256(uint64_t firstSeed);

                void seed();
                void seed(uint64_t seed);
                void discard(uint64_t amount);
                bool Bool();
                uint64_t min();
                uint64_t max();
                uint64_t operator()();
            };

        }

    }

}

