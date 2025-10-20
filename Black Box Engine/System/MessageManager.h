#pragma once

#include <vector>
#include <string>
#include <functional>

#include "StringHash.h"
#include "Observer.h"

namespace BlackBoxEngine
{
    class Actor;
    struct Message;

    using MessageIdType = uint64_t;

    namespace EngineMessages
    {
        inline static constexpr MessageIdType kCollided = StringHash("Collided");
        inline static constexpr MessageIdType kAny = StringHash("Any");
    };

    /**
     * MessageIdType messageId
     * Actor* pSender
     * Actor* pReciever
     */
    struct Message
    {
        MessageIdType messageId = 0;
        Actor* pSender;
        Actor* pReciever = nullptr;
    };

    bool operator==(const Message& left, const Message& right);
    bool operator!=(const Message& left, const Message& right);

    class MessagingManager : public BB_Observer< std::function<void(Message&)>, MessageIdType , MessageIdType >
    {
        friend class BlackBoxManager;
        std::vector<Message> m_eventQueue;
        IdType m_anyMessageId = 0;
        std::vector < std::pair<CallbackIdType, ListenerType>> m_AnyMessageListeners;
    
    private:
        void SendQueuedMessages();

    public:
        void EnqueueMessage(Message message);
        void EnqueueMessage(MessageIdType messageId, Actor* pSender, Actor* pIntendedReciver = nullptr);
        void EnqueueMessage(const char* pUnhashedMessage, Actor* pSender, Actor* pIntendedReciver = nullptr);
      
        CallbackIdType RegisterForAnyMessage(ListenerType&& listerFunction);
        void RemoveAnyMessageListener(CallbackIdType callbackId);

        /**
         * @brief 
         * @param pMessage : The string you want to listen for
         * @param listener : a void(Message&) function
         * @return 
         */
        CallbackIdType RegisterListenerString(const char* pMessage, ListenerType&& listener);
    };


}