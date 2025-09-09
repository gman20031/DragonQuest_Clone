#pragma once

#include <chrono>
#include <string>
#include <unordered_map>

namespace BlackBoxEngine
{
    class SimpleTimer
    {
        using ClockType = std::chrono::high_resolution_clock;
        using TimerMapType = std::unordered_map<std::string, ClockType::time_point >;
        TimerMapType timerMap;

        static TimerMapType& Map();
        ClockType::time_point m_startTime;

    public:

        static void StartGlobalTimer(const std::string& name);
        static std::chrono::milliseconds StopTimer(const std::string& name);

        void StartTimer() { m_startTime = ClockType::now(); }
        double GetDeltaTime() const;
    };


}



