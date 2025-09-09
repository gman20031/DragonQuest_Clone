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

    class InputManager
    {
    public:
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

        std::unordered_set<KeyCode> m_keyCodes;
        InputObserver m_observers[(size_t)InputType::kCount];

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


    };
};