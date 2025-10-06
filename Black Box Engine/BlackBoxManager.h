#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <atomic>

#include "Graphics/Window.h"
#include "Graphics/Renderer.h"
#include "Graphics/Camera.h"
#include "Audio/AudioManager.h"
#include "Actors/ActorManager.h"
#include "Input/InputManager.h"
#include "System/MessageManager.h"
#include "Interface/InterfaceManger.h"
#include "Actors/Collision/CollisionManager.h"

namespace BlackBoxEngine
{
    class BlackBoxManager
    {
    public:
        using WindowPtr = std::unique_ptr<BB_Window>;
        using GameUpdateFunc = std::function<void( void )>;
        using EngineInitOptions = uint8_t;
        static constexpr EngineInitOptions kNone        = 0b00000;
        static constexpr EngineInitOptions kActorSystem = 0b00001;
        static constexpr EngineInitOptions kInput	    = 0b00010;
        static constexpr EngineInitOptions kCollision   = 0b00100;
        static constexpr EngineInitOptions kMessaging   = 0b01000;
        static constexpr EngineInitOptions kAudio       = 0b11000;
        static constexpr EngineInitOptions kAll	        = 0b11111;

    private:
        GameUpdateFunc m_gameUpdateFunction;
        std::recursive_mutex m_engineMutex;
        inline static constexpr float kCollisionBufferSize = 0;
        static BlackBoxManager* m_pSingletonManager;
        WindowPtr m_pWindow;
        double m_deltaTime = 0;
        EngineInitOptions m_engineOptions = kNone;

    public:
        bool m_keepRunning = false;
        CollisionManager* m_pCollisionManager = nullptr;
        ActorManager*     m_pActorManager = nullptr;
        InputManager*     m_pInputManager = nullptr;
        AudioManager*     m_pAudioManager = nullptr;
        MessagingManager* m_pMessagingManager = nullptr;
        InterfaceManager* m_pInterfaceManager = nullptr;
        BB_Camera*        m_pMainCamera = nullptr;
    public:
        ~BlackBoxManager();
        BlackBoxManager(const BlackBoxManager&) = delete;
        BlackBoxManager(BlackBoxManager&&) = delete;
        void operator=(const BlackBoxManager&) = delete;
        void operator=(BlackBoxManager&&) = delete;

        const WindowPtr& GetWindow() { return m_pWindow; }
        [[nodiscard("index not caught after creating window")]] void CreateWindow(
            const char* title,
            int xPos, int yPos,
            int width,
            int height,
            BB_Window::WindowFlags flags
        );
        double GetDeltaTime() const { return m_deltaTime; }
        bool IsSystemEnabled(EngineInitOptions option) const;
        void SetGameUpdate( GameUpdateFunc&& callback );

        [[nodiscard("Manager not being used")]] static BlackBoxManager* Get();
        static BlackBoxManager* NewAndInitEngine(EngineInitOptions options = kAll);
        static BlackBoxManager* NewSingleton();
        static void DeleteSingleton();

        int RunEngine();
        int InitEngine(EngineInitOptions options = kAll);
    private: // functions
        void HandleSdlEvents();
        void CheckEngineInitialized();
        void UpdateSubsytems();
        BlackBoxManager() = default;
    };
}

