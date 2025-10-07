#include "Delay.h"

#include <algorithm>
#include <ranges>

namespace BlackBoxEngine
{
    void DelayedCallbackManager::Update( double deltaMS )
    {
        m_currentTime += deltaMS;
        while ( !m_callbacks.empty() && m_currentTime >= m_callbacks.front().callTimeSeconds )
        {
            m_callbacks.front().callback();
            if ( m_callbacks.front().repeatTime > 0 )
            {
                m_callbacks.emplace_back( m_callbacks.front() );
                m_callbacks.back().callTimeSeconds = m_currentTime + m_callbacks.back().repeatTime;
            }
            std::ranges::pop_heap( m_callbacks, &HeapCompare);
            m_callbacks.pop_back();
        }
    }

    uint64_t DelayedCallbackManager::AddCallback(
        DelayedCallback::Callback&& callback,
        std::chrono::seconds delay,
        std::chrono::seconds repeat )
    {
        using namespace std::chrono;
        return AddCallback(
            std::forward<DelayedCallback::Callback>( callback ),
            duration_cast<milliseconds>(delay),
            duration_cast<milliseconds>(repeat) 
        );
    }

    uint64_t DelayedCallbackManager::AddCallback(
        DelayedCallback::Callback&& callback,
        std::chrono::milliseconds delay,
        std::chrono::milliseconds repeat )
    {
        using namespace std::chrono;
        return AddCallback(
            std::forward<DelayedCallback::Callback>( callback ),
            static_cast<double>(delay.count()) , static_cast<double>(repeat.count())
        );
    }

    uint64_t DelayedCallbackManager::AddCallback(
        DelayedCallback::Callback&& callback, double delayMS, double repeatMS )
    {
        uint64_t id = NextId();
        m_callbacks.emplace_back( 
            id,
            std::forward<DelayedCallback::Callback>( callback ),
            m_currentTime + ( delayMS ) ,
            repeatMS
        );
        std::ranges::make_heap( m_callbacks, &HeapCompare );
        return id;
    }

    bool DelayedCallbackManager::RemoveCallback( uint64_t id )
    {
        for ( auto it = m_callbacks.begin(); it != m_callbacks.end() ; ++it)
        {
            if( it->id == id )
            {
                m_callbacks.erase( it );
                std::ranges::make_heap( m_callbacks, &HeapCompare );
                return true;
            }
        }
        return false;
    }

    uint64_t DelayedCallbackManager::NextId()
    {
        if( m_unusedIds.empty() )
            return m_nextId++;
        uint64_t id = m_unusedIds.back();
        m_unusedIds.pop_back();
        return id;
    }

}

