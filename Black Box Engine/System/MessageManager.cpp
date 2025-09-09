#include "MessageManager.h"

#include "../System/StringHash.h"

namespace BlackBoxEngine
{
    void MessagingManager::EnqueueMessage(Message message)
    {
        m_eventQueue.emplace_back(message);
    }

    void MessagingManager::EnqueueMessage(MessageIdType messageId, Actor* pSender, Actor* pIntendedReciver)
    {
        m_eventQueue.emplace_back(messageId, pSender, pIntendedReciver);
    }

    void MessagingManager::EnqueueMessage(const char* pUnhashedMessage, Actor* pSender, Actor* pIntendedReciver)
    {
        m_eventQueue.emplace_back((MessageIdType)StringHash(pUnhashedMessage), pSender, pIntendedReciver);
    }

    void MessagingManager::SendQueuedMessages()
    {
        for (auto& message : m_eventQueue)
        {
            for (auto& pair : m_AnyMessageListeners)
                pair.second(message);
            PushEvent(message.messageId, message);
        }
        m_eventQueue.clear();
    }

    MessagingManager::CallbackIdType MessagingManager::RegisterForAnyMessage(ListenerType&& listerFunction)
    {
        MessageIdType id = m_anyMessageId;
        ++m_anyMessageId;
        m_AnyMessageListeners.emplace_back(id, std::forward<ListenerType>(listerFunction));
        return id;
    }

    void MessagingManager::RemoveAnyMessageListener(CallbackIdType callbackId)
    {
        for (auto it = m_AnyMessageListeners.begin(); it != m_AnyMessageListeners.end(); ++it)
        {
            if (it->first == callbackId)
            {
                *it = m_AnyMessageListeners.back();
                m_AnyMessageListeners.pop_back();
                return;
            }
        }
        BB_LOG(LogType::kError, "attempted to remove listener that was not registerd");
    }

    MessagingManager::CallbackIdType MessagingManager::RegisterListenerString(const char* pMessage, ListenerType&& listener)
    {
        return RegisterListener(StringHash(pMessage), std::forward<ListenerType>(listener) );
    }

    bool operator==(const Message& left, const Message& right)
    {
        return left.messageId == right.messageId;
    }

    bool operator!=(const Message& left, const Message& right)
    {
        return left.messageId != right.messageId;
    }
}
