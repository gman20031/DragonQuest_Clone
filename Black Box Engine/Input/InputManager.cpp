#include <SDL.h>

#include "InputManager.h"

namespace BlackBoxEngine
{
    InputManager::InputManager()
    {
        m_keyCodes.reserve(static_cast<size_t>(KeyCode::kCount));
}

    void InputManager::AddKeyDown(KeyCode key)
    {
        if (m_keyCodes.contains(key))
            return;

        m_keyCodes.emplace(key);
        m_observers[(size_t)InputType::kKeyDown].PushEvent(key);
        if constexpr (kLogInputData)
            BB_LOG(LogType::kMessage, "Key down : ", key);
    }

    void InputManager::RemoveKeyDown(KeyCode key)
    {
        if (!m_keyCodes.contains(key))
        {
            BB_LOG(LogType::kError, "Attempting to remove key that was not pressed");
            return;
        }

        m_keyCodes.erase(key);
        m_observers[(size_t)InputType::kKeyUp].PushEvent(key);
        if constexpr (kLogInputData)
            BB_LOG(LogType::kMessage, "Key up : ", key);
    }

    bool InputManager::IsKeyDown(KeyCode key) const
    {
        return m_keyCodes.contains(key);
    }

    FVector2 InputManager::GetMousePosition() const
    {
        float x = 0, y = 0;
        SDL_GetMouseState(&x, &y);
        return FVector2(x,y);
    }

    InputManager::CallBackId InputManager::SubscribeToKey( KeyCode key, InputType type , Callback&& function)
    {
        return m_observers[(size_t)type].RegisterListener( key , std::forward<Callback>(function));
    }

    void InputManager::UnsubscribeKey(CallBackId id, InputType type)
    {
        m_observers[(size_t)type].RemoveListener(id);
    }

    void InputManager::UnsubscribeKeyWithCode(CallBackId id, InputType type, KeyCode key)
    {
        m_observers[(size_t)type].RemoveListenerWithEvent(id, key);
    }

    void InputManager::Update()
    {
        for (KeyCode code : m_keyCodes)
        {
            m_observers[(size_t)InputType::kKeyHeld].PushEvent(code);
            if constexpr (kLogInputData)
                BB_LOG(LogType::kMessage, "Key held : ", code);
        }
    }

}

