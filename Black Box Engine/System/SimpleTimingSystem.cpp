#include "SimpleTimingSystem.h"

namespace BlackBoxEngine
{
    SimpleTimer::TimerMapType & SimpleTimer::Map()
    {
        static TimerMapType map;
        return map;
    }

    void SimpleTimer::StartGlobalTimer(const std::string& name)
    {
        Map()[name] = ClockType::now();
    }

    std::chrono::milliseconds SimpleTimer::StopTimer(const std::string& name)
    {
        auto duration = ClockType::now() - Map()[name];
        return  std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    }

    double SimpleTimer::GetDeltaTime() const
    {
        using namespace std::chrono;
        nanoseconds nanoDelta = (ClockType::now() - m_startTime);
        double secondsDelta = (double)nanoDelta.count() / (double)std::nano::den;
        return secondsDelta;
    }

}