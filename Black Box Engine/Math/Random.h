#pragma once

#include <random>
#include <concepts>

#include "../System/Debugging.h"
#include "../System/Log.h"
#include "NumericalConcepts.h"

#include "RandomEngines.h"

namespace BlackBoxEngine
{
	namespace Random
	{
        //////////////////////////////////////////////////////////////////////////////////////////
        /// BlackBox PRNG Machine
        //////////////////////////////////////////////////////////////////////////////////////////

		/**
		 * @brief A Psuedo random number generator that is powered by whichever engine is set.
         * @brief All random generation should be done through a PRNG Machine. There pre-templated defintions, as well as static machines available.
         * @brief If no m_seed is set, the machine defaults to using std::random_device()(), which may or may not be desirable depending on the machine.
		 * @tparam EngineType - the underlying engine that is used for all the random generation.
		 * @tparam ReturnType - the standard integral return type for random calls.
		 */
		template<PRNG_Engine EngineType, std::integral ReturnType>
		class Machine
		{
            using ThisType = Machine<EngineType, ReturnType>;
		private:
			EngineType m_engine;
			size_t m_seed;
		public:
			Machine() : m_seed(std::random_device()()) {Reset(m_seed);}
			Machine(size_t seed) : m_seed(seed) { Reset(m_seed);}

            EngineType& GetEngineRef() { return m_engine; }
            // @brief Reseeds the engine with std::random_device()()
            void RandomReset() { m_engine.seed(std::random_device()()); }
            // @brief Reseeds the engine to the previously set m_seed
			void Reset() { m_engine.seed(m_seed); }
            // @brief Reseeds the engine to the new m_seed value
			void Reset(size_t newSeed);
            // @brief Generates and returns random value.
			auto Max() { return EngineType::max(); }

            ReturnType operator()() { return m_engine(); }

            // integers
            /**
             * @brief Returns a random DataType value between min and max inclusive. See GetRandomInRange.
             * @tparam Integral DataType : handles both the param and return type
             */
            template<std::integral DataType = ReturnType>  
            DataType GetRandomInRange(DataType min, DataType max);
            /**
             * @brief Returns a random DataType value between min and max excluding the min and max. See GetRandomInRangeExclusive.
             * @tparam Integral DataType : handles both the param and return type
             */
            template<std::integral DataType = ReturnType>
            DataType GetRandomInRangeExclusive(DataType min, DataType max);
            
            template<std::integral DataType = ReturnType>
            DataType GetRandomInRange(DataType max);
            template<std::integral DataType = ReturnType>
            DataType GetRandomInRangeExclusive(DataType max);

            // floating point, no exlusive variants

            template<std::floating_point DataType>
            DataType GetRandomInRange(DataType min, DataType max);
            template<std::floating_point DataType>
            DataType GetRandomInRange(DataType max);

            // boolean
            bool GetRandomBool();
        };

        template<PRNG_Engine EngineType, std::integral Return_t>
        inline void Machine<EngineType, Return_t>::Reset(size_t newSeed)
        {
            m_seed = newSeed;
            m_engine.Seed(m_seed);
            BB_LOG(LogType::kMessage, "RNG engine Seed has been set to:", m_seed);
        }

        template<PRNG_Engine EngineType, std::integral ReturnType>
        template<std::integral DataType>
        inline DataType BlackBoxEngine::Random::Machine<EngineType, ReturnType>::GetRandomInRange(DataType min, DataType max)
        {
            DataType range = max - min;
            return static_cast<DataType>(min + (m_engine() % (range + 1)));
        }

        template<PRNG_Engine EngineType, std::integral ReturnType>
        template<std::integral DataType>
        inline DataType Machine<EngineType, ReturnType>::GetRandomInRangeExclusive(DataType min, DataType max)
        {
            DataType range = max - min;
            return static_cast<DataType>((min - 1) + (m_engine() % range));
        }

        template<PRNG_Engine EngineType, std::integral ReturnType>
        template<std::integral DataType>
        inline DataType Machine<EngineType, ReturnType>::GetRandomInRange(DataType max)
        {
            return static_cast<DataType>(m_engine() % (max + 1));
        }

        template<PRNG_Engine EngineType, std::integral ReturnType>
        template<std::integral DataType>
        inline DataType Machine<EngineType, ReturnType>::GetRandomInRangeExclusive(DataType max)
        {
            return static_cast<DataType>(m_engine() % max);
        }

        template<PRNG_Engine EngineType, std::integral ReturnType>
        template<std::floating_point DataType>
        inline DataType BlackBoxEngine::Random::Machine<EngineType, ReturnType>::GetRandomInRange(DataType min, DataType max)
        {
            DataType engineOutput = static_cast<DataType>(m_engine());
            DataType output = engineOutput / m_engine.Max();
            DataType range = max - min;
            return output * range;
        }

        template<PRNG_Engine EngineType, std::integral ReturnType>
        template<std::floating_point DataType>
        inline DataType Machine<EngineType, ReturnType>::GetRandomInRange(DataType max)
        {
            DataType engineOutput = static_cast<DataType>(m_engine());
            DataType output = engineOutput / m_engine.Max();
            return output * max;
        }

        template<PRNG_Engine EngineType, std::integral ReturnType>
        inline bool Machine<EngineType, ReturnType>::GetRandomBool()
        {
            if constexpr (BoolSpecialized<EngineType>)
                return m_engine.Bool();
            else
            {
                ReturnType rand = m_engine();
                rand &= 0x1;
                return rand;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////////
        /// Templated Defaults
        //////////////////////////////////////////////////////////////////////////////////////////
        
        using MachineXoshiro256 = Machine<Engine::Xoshiro256, uint64_t>;
        using MachineSplitMix64 = Machine<Engine::SplitMix64, uint64_t>;

        //////////////////////////////////////////////////////////////////////////////////////////
        /// Public Functions, For use with static Random Engines
        //////////////////////////////////////////////////////////////////////////////////////////

        namespace Global
        {
            MachineXoshiro256& BB_Xoshiro256();
            MachineSplitMix64& BB_SplitMix64();

        } // Global

        void TestRNG();

	} // random

}// BlackBoxEngine