#pragma once

#include <functional>
#include <chrono>

#include "../System/StringHash.h"

namespace BlackBoxEngine
{
    struct DelayedCallback
    {
        using Callback = std::function<void()>;

        uint64_t id = 0;
        Callback callback;
        double callTimeSeconds;
        double repeatTime = 0;
    };

    class DelayedCallbackManager
    {
        inline static std::vector<DelayedCallback> m_callbacks;
        inline static std::vector<uint64_t> m_unusedIds;
        inline static double m_currentTime = 0;
        inline static uint64_t m_nextId = 0;
    public:
        static void Update( double deltaMS );

        static uint64_t AddCallback( DelayedCallback::Callback&& callback, std::chrono::seconds delay, std::chrono::seconds repeat = {} );
        static uint64_t AddCallback( DelayedCallback::Callback&& callback, std::chrono::milliseconds delay, std::chrono::milliseconds repeat = {} );
        static uint64_t AddCallback( DelayedCallback::Callback&& callback, double delayMS, double repeatMS = 0 );
        
        static bool RemoveCallback( uint64_t id );

    private:
        static bool HeapCompare(
            const DelayedCallback& left,
            const DelayedCallback& right)
        {
            return left.callTimeSeconds > right.callTimeSeconds;
        }
        static uint64_t NextId();
    };
}
