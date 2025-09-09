#pragma once

#include <concepts>
#include <unordered_map>

#include "Log.h"

/*
General Idea of this pattern:
    Be able to hold and remember many different listeners and what they are listening for.
    when the thing they are listening for occurs, call the listeners to tell them it occured.

*/

namespace BlackBoxEngine
{
    template<class Type>
    concept Comparable = requires (Type lhs, Type rhs)
    {
        { lhs == rhs } -> std::convertible_to<bool>;
        { lhs != rhs } -> std::convertible_to<bool>;
    };

    template <class Listener, Comparable Event, std::integral CallbackId = uint64_t>
    class BB_Observer
    {
    public:
        using ListenerType = Listener;
        using CallbackIdType = CallbackId;
        using EventType = Event;

        using FunctionMap = std::unordered_map< CallbackId, Listener>;
        using EventMap = std::unordered_map<Event, FunctionMap>;
        using IdType = CallbackId;
        using EventType = Event;

    protected:
        EventMap m_eventMap;
        CallbackId m_currentId = 0;

    public:
        // Registering
        CallbackId RegisterListener(const Event& event , Listener&& callbackFunction);

        // Removal
        void RemoveListener(CallbackId callbackId);
        void RemoveListenerWithEvent(CallbackId callbackId, const Event& event);

        // Checking 
        bool IsListeningFor(const Event& event);

        // Sending
        void PushEvent(const Event& event);
        template<typename... Args> void PushEvent(const Event& event, Args&&... args);
    };

    template<class Listener, Comparable Event, std::integral CallbackId>
    inline CallbackId BB_Observer<Listener, Event, CallbackId>::RegisterListener(const Event& event , Listener&& callbackFunction)
    {
        m_eventMap[event].emplace(m_currentId, std::forward<Listener>(callbackFunction));
        return m_currentId++;
    }

    /**
     * @brief Loops through every function stored by the observer to remove the correct listener function
     * @param callbackId : Id returned by RegisterListener(), tied to the listening function
     */
    template<class Listener, Comparable Event, std::integral CallbackId>
    inline void BB_Observer<Listener, Event, CallbackId>::RemoveListener(CallbackId callbackId)
    {
        for (auto& [event, map] : m_eventMap)
        {
            auto it = map.find(callbackId);
            if (it == map.end())
                continue;
            map.erase(it);
            return;
        }
        BB_LOG(LogType::kWarning, "Attempted to remove listener that doesnt exist ID: ", callbackId);
    }

    /**
     * @brief attempts to remove the function tied to callbackId, using event if the listening event is known.
     * @param callbackId: Id returned by RegisterListener(), tied to the listening function
     * @param event: event the function is tied to
     */
    template<class Listener, Comparable Event, std::integral CallbackId>
    inline void BB_Observer<Listener, Event, CallbackId>::RemoveListenerWithEvent(CallbackId callbackId, const Event& event)
    {
        auto it = m_eventMap.find(event);
        if (it == m_eventMap.end())
        {
            RemoveListener(callbackId); // do slow version if the event is not here.
            return;
        }
        auto innerIt = it->second.find(callbackId);
        if (innerIt == it->second.end())
        {
            BB_LOG(LogType::kWarning, "Attempted to remove listener that doesnt exist ID: ", callbackId, " - Event - ", event);
            return;
        }
        it->second.erase(innerIt);
    }

    /**
     * @brief Calls every function listening for event, passing in void.
     * @param event : event functions are tied to with RegisterListener()
     */
    template<class Listener, Comparable Event, std::integral CallbackId>
    inline void BB_Observer<Listener, Event, CallbackId>::PushEvent(const Event& event)
    {
        auto it = m_eventMap.find(event);
        if (it == m_eventMap.end())
            return;
        for (auto& [id, invocableObject] : it->second)
        {
            invocableObject();
        }
    }

    /**
     * @brief Calls every function listening for event, passing in all arguements after event.
     * @param event : event functions are tied to with RegisterListener()
     * @param args : all arguements to be passed to the function call
     */
    template<class Listener, Comparable Event, std::integral CallbackId>
    template<typename ...Args>
    inline void BB_Observer<Listener, Event, CallbackId>::PushEvent(const Event& event, Args&& ...args)
    {
        auto it = m_eventMap.find(event);
        if (it == m_eventMap.end())
            return;
        for (auto& [id, function] : it->second)
        {
            function(std::forward<Args>(args)...);
        }
    }

    template<class Listener, Comparable Event, std::integral CallbackId>
    inline bool BB_Observer<Listener, Event, CallbackId>::IsListeningFor(const Event& event)
    {
        auto it = m_eventMap.find(event);
        if (it == m_eventMap.end())
            return false;
        return true;
    }

};