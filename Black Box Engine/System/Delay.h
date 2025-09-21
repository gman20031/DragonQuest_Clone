#pragma once

#include <unordered_map>
#include <chrono>

#include "../System/StringHash.h"

namespace BlackBoxEngine
{
    using DelayFunction = uint32_t(*)(void* userData, uint32_t timerId, uint32_t interval);
    uint32_t Delay(uint32_t milisecondDelay, DelayFunction callbackFunction, void* userData);
    uint32_t Delay(std::chrono::milliseconds milisecondDelay, DelayFunction callbackFunction, void* userData);
    uint32_t Delay(std::chrono::seconds secondDelay, DelayFunction callbackFunction, void* userData);


    ///**
    // * @brief see https://wiki.libsdl.org/SDL3/SDL_AddTimer
    // */
    //class Delay
    //{
    //    //todo: think about implementing a timer system myself.
    //public:
    //private:
    //    //friend class BlackBoxManager;
    //    //using CallbackMap = std::unordered_map<uint32_t, FunctionPtr>;

    //    //CallbackMap & GetCallbacks();
    //    //Delay();
    //public:
    //};

}
