#pragma once

#include "../System/Debugging.h"
#include "../System/Log.h"
#include "NumericalConcepts.h"

#include <random>
#include <concepts>

namespace BlackBoxEngine
{

	namespace Random
	{
		template<typename Engine_t, typename Return_t>
		class Engine
		{
		private:
			Engine_t m_engine;
			size_t seed;
		public:
			Engine();
			Engine(size_t seed);

			void Reset();
			void Reset(size_t newSeed);
			void Discard(size_t amount);
			auto Max() { return Engine_t::max(); }

			Return_t operator()();
		};

		using EngineMT64 = Engine<std::mt19937_64, uint64_t >;

		// (min , max) inclusive
		template<std::integral data_t, typename Engine_t>
		inline data_t GetRandomInRange(data_t min, data_t max, Engine_t& engine)
		{
			return (min + engine() % (max + 1));
		}
		template<std::integral data_t, typename Engine_t>
		inline data_t GetRandomInRange(data_t max, Engine_t& engine)
		{
			return engine() % (max + 1);
		}
		template<std::floating_point data_t, typename Engine_t>
		inline data_t GetRandomInRange(data_t min, data_t max, Engine_t& engine)
		{
			data_t engineOutput = static_cast<data_t>(engine());
			data_t output = engineOutput / engine.Max();
			data_t range = max - min;
			return output * range;
		}
		template<std::floating_point data_t, typename Engine_t>
		inline data_t GetRandomInRange(data_t max, Engine_t& engine)
		{
			int engineOutput = engine();
			data_t output = data_t(engineOutput) / engine.Max();
			return output * max;
		}

		namespace Static
		{
			class mt_64
			{
			public:
				static std::mt19937_64& MT64()
				{
					static std::mt19937_64 s_mt64(time(0));
					return s_mt64;
				}


				template<std::integral data_t>
				static data_t GetRandomInRange(data_t min, data_t max);

			};

			// (min , max) inclusive
			template<std::integral data_t>
			inline data_t mt_64::GetRandomInRange(data_t min, data_t max)
			{
				return (min + MT64()() % (max + 1));
			}
		}


		template<typename Engine_t, typename Return_t>
		inline Engine<Engine_t, Return_t>::Engine()
			:seed(std::random_device()())
		{
			std::cout << "seedSet: " << seed << '\n';
		}
		template<typename Engine_t, typename Return_t>
		inline Engine<Engine_t, Return_t>::Engine(size_t seed)
			: seed(std::random_device()())
		{
			std::cout << "seedSet: " << seed << '\n';
		}
		template<typename Engine_t, typename Return_t>
		inline void Engine<Engine_t, Return_t>::Reset()
		{
			seed = std::random_device()();
			m_engine.seed(seed);
		}
		template<typename Engine_t, typename Return_t>
		inline void Engine<Engine_t, Return_t>::Reset(size_t newSeed)
		{
			seed = newSeed;
			m_engine.seed(seed);
			std::cout << "seedSet: " << seed << '\n';

		}
		template<typename Engine_t, typename Return_t>
		inline void Engine<Engine_t, Return_t>::Discard(size_t amount)
		{
			m_engine.discard(amount);
		}
		template<typename Engine_t, typename Return_t>
		inline Return_t Engine<Engine_t, Return_t>::operator()()
		{
			return m_engine();
		}
	}

}// BlackBoxEngine