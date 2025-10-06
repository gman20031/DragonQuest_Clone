#include "Random.h"

#include "../System/Log.h"
#include "../System/StringHash.h"

namespace BlackBoxEngine
{
    namespace Random
    {
        namespace Global
        {
            MachineXoshiro256& BB_Xoshiro256()
            {
                static MachineXoshiro256 s_xoshiro;
                return s_xoshiro;
            }

            MachineSplitMix64& BB_SplitMix64()
            {
                static MachineSplitMix64 s_splitMix;
                return s_splitMix;
            }
        }

        void TestRNG()
        {
            MachineXoshiro256 rngMachine;

            std::cout << "\n\nTesting operator()\n";
            for (size_t i = 0; i < 10; ++i)
                std::cout << rngMachine() << ' ';
            
            std::cout << "\n\nTesting RandomInRange(10,20)\n";
            for (size_t i = 0; i < 10; ++i)
                std::cout << rngMachine.GetRandomInRange(10, 20) << ' ';
            rngMachine.
            std::cout << "\n\nTesting RandomInRange(10)\n";
            for (size_t i = 0; i < 10; ++i)
                std::cout << rngMachine.GetRandomInRange(10) << ' ';

            std::cout << "\n\nTesting GetRandomInRangeExclusive(10)\n";
            for (size_t i = 0; i < 10; ++i)
                std::cout << rngMachine.GetRandomInRangeExclusive(10) << ' ';

            std::cout << "\n\nTesting GetRandomInRangeExclusive(10,20)\n";
            for (size_t i = 0; i < 10; ++i)
                std::cout << rngMachine.GetRandomInRangeExclusive(10,20) << ' ';

            std::cout << "\n\nTesting GetRandomInRange(0.25f , 1.00f)\n";
            for (size_t i = 0; i < 10; ++i)
                std::cout << rngMachine.GetRandomInRange(.25f, 1.00f) << ' ';

            std::cout << "\n\nTesting GetRandomInRange(1.00f)\n";
            for (size_t i = 0; i < 10; ++i)
                std::cout << rngMachine.GetRandomInRange(1.00f) << ' ';

            std::cout << "\n\nTesting GetRandomBool()\n";
            for (size_t i = 0; i < 10; ++i)
                std::cout << (bool)rngMachine.GetRandomBool() << ' ';

            std::cout << "\n";
            ConsoleManip::Pause();
        }
    }
}

