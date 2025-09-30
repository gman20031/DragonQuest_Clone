#pragma once

#include <unordered_map>
#include <functional>
#include <chrono>

#include "../System/StringHash.h"

namespace BlackBoxEngine
{
    using DelayFunction = uint32_t(*)(void* userData, uint32_t timerId, uint32_t interval);
    // @brief see https://wiki.libsdl.org/SDL3/SDL_AddTimer
    uint32_t Delay(uint32_t milisecondDelay, DelayFunction callbackFunction, void* userData);
    uint32_t Delay(std::chrono::milliseconds milisecondDelay, DelayFunction callbackFunction, void* userData);
    uint32_t Delay(std::chrono::seconds secondDelay, DelayFunction callbackFunction, void* userData);
    
    // Callable must return uint32_t for when it should be called next
    template<std::invocable Callable>
    uint32_t Delay(uint32_t miliseconds, Callable callback)
    {
        using TempMap = std::unordered_map<uint32_t, Callable>;
        static TempMap s_tempStorage;

        DelayFunction function = [](void* pMap, [[maybe_unused]] uint32_t timerId, [[maybe_unused]] uint32_t interval) -> uint32_t
            {
                TempMap* pTempMap = static_cast<TempMap*>(pMap);
                return pTempMap->at(timerId)();
            };

        auto id = Delay(miliseconds, function, &s_tempStorage);
        s_tempStorage.emplace(id, callback);

        return id;
    }
 
    bool StopDelay( uint32_t id );
}
