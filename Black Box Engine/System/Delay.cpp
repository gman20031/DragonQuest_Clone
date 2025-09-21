#include "Delay.h"

#include <SDL3/SDL_timer.h>

namespace BlackBoxEngine
{

    uint32_t Delay(uint32_t milisecondDelay, DelayFunction callbackFunction, void* userData)
    {
        return SDL_AddTimer(milisecondDelay, callbackFunction, userData);
    }

    uint32_t Delay(std::chrono::milliseconds milisecondDelay, DelayFunction callbackFunction, void* userData)
    {
        return SDL_AddTimer(static_cast<uint32_t>(milisecondDelay.count()), callbackFunction, userData);
    }

    uint32_t Delay(std::chrono::seconds secondDelay, DelayFunction callbackFunction, void* userData)
    {
        using namespace std::chrono; 
        auto milisecondDelay = duration_cast<milliseconds>(secondDelay);
        return SDL_AddTimer(static_cast<uint32_t>(milisecondDelay.count()), callbackFunction, userData);
    }

}

//CallbackMap& BlackBoxEngine::Delay::GetCallbacks()
//{
//    SDL_TimerCallback  
//}

