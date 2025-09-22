#pragma once

#include <cassert>
#include <cstddef>
#include <functional>
#include <bitset>
#include <unordered_set>

#include "KeyCodes.h"
#include "../System/Observer.h"
#include "../Math/FVector2.h"

namespace BlackBoxEngine
{
    class UserInterface;

    class InputManager
    {
    public:
        friend class UserInterface;
        enum class InputType
        {
            kKeyDown,
            kKeyUp,
            kKeyHeld,
            kCount,
        };
    private:
        using Callback = std::function<void()>;
        using CallBackId = uint64_t;
        using InputObserver = BB_Observer< Callback, KeyCode , CallBackId>;
        inline static constexpr bool kLogInputData = false;

        struct InputTarget
        {
            InputObserver m_keyDown;
            InputObserver m_keyUp;
            InputObserver m_keyHeld;
        };

        std::unordered_set<KeyCode> m_keyCodes;
        InputTarget m_gameInputTarget;
        InputTarget* m_pInputTarget;

        bool m_inputCanOccur = true;
    public:
        InputManager();

        void AddKeyDown(KeyCode key);
        void RemoveKeyDown(KeyCode key);

        bool IsKeyDown(KeyCode key) const;
        FVector2 GetMousePosition() const;

        CallBackId SubscribeToKey(KeyCode key, InputType type, Callback&& function);
        void UnsubscribeKey(CallBackId id, InputType type);
        void UnsubscribeKeyWithCode(CallBackId id, InputType type, KeyCode key);
        void Update();

        void StopAllInput() { m_inputCanOccur = false; }
        void ResumeInput() { m_inputCanOccur = true; } 
        void SwapInputTargetToInterface( UserInterface* pInterface);
        void SwapInputToGame();
    };
};