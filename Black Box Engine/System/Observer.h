#pragma once
/*
General Idea of this pattern:
    Be able to hold and remember many different listeners and what they are listening for.
    when the thing they are listening for occurs, call the listeners to tell them it occured.

    In specifics of the engine, this forms the basis of both the input and messaging system.
*/

#include <concepts>
#include <unordered_map>

#include "Log.h"


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
        /**
         * @brief Moves the callback function into the observer system, and will call the function when the coresponding
         * event is pushed using PushEvent()
         * @return The key used for listener lookup and removal.
         */
        CallbackId RegisterListener(const Event& event , Listener&& callbackFunction);

        // Removal
        /**
         * @brief Loops through every function stored by the observer to remove the correct listener function
         * @param callbackId : Id returned by RegisterListener(), tied to the listening function
         */
        void RemoveListener(CallbackId callbackId);
        /**
         * @brief attempts to remove the function tied to callbackId, using event if the listening event is known.
         * @param callbackId: Id returned by RegisterListener(), tied to the listening function
         * @param event: event the function is tied to
         */
        void RemoveListenerWithEvent(CallbackId callbackId, const Event& event);
        /**
         * @brief Removes every function that is currently listening with this observer.
         * @brief RemoveListener() can be called after this without anything happening.
         */
        void ClearListeners();

        // Checking 
        bool IsListeningFor(const Event& event);
        bool ContainsCallback( const CallbackId );

        // Sending
        /**
         * @brief Calls every function listening for event, passing in void.
         * @param event : event functions are tied to with RegisterListener()
         */
        void PushEvent(const Event& event);
        /**
         * @brief Calls every function listening for event, passing in all arguements after event.
         * @param event : event functions are tied to with RegisterListener()
         * @param args : arguments to be passed to the function call
         */
        template<typename... Args> void PushEvent(const Event& event, Args&&... args);
    };

    template<class Listener, Comparable Event, std::integral CallbackId>
    inline CallbackId BB_Observer<Listener, Event, CallbackId>::RegisterListener(const Event& event , Listener&& callbackFunction)
    {
        m_eventMap[event].emplace(m_currentId, std::forward<Listener>(callbackFunction));
        return m_currentId++;
    }

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
        //BB_LOG(LogType::kWarning, "Attempted to remove listener that doesnt exist ID: ", callbackId);
    }

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

    template<class Listener, Comparable Event, std::integral CallbackId>
    inline void BB_Observer<Listener, Event, CallbackId>::ClearListeners()
    {
        m_eventMap.clear();
    }

    template<class Listener, Comparable Event, std::integral CallbackId>
    inline void BB_Observer<Listener, Event, CallbackId>::PushEvent(const Event& event)
    {
        auto it = m_eventMap.find(event);
        if (it == m_eventMap.end())
            return;
        for (auto& [id, invocableObject] : it->second)
            invocableObject();
    }

    template<class Listener, Comparable Event, std::integral CallbackId>
    template<typename ...Args>
    inline void BB_Observer<Listener, Event, CallbackId>::PushEvent(const Event& event, Args&& ...args)
    {
        auto it = m_eventMap.find(event);
        if (it == m_eventMap.end())
            return;
        for (auto& [id, function] : it->second)
            function(std::forward<Args>(args)...);
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