#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "Graphics/Window.h"
#include "Graphics/Renderer.h"
#include "Graphics/Camera.h"
#include "Audio/AudioManager.h"
#include "Actors/ActorManager.h"
#include "Input/InputManager.h"
#include "System/MessageManager.h"
#include "Actors/Collision/CollisionManager.h"

namespace BlackBoxEngine
{
    class BlackBoxManager
    {
    public:
        using WindowPtr = std::unique_ptr<BB_Window>;
        using EngineInitOptions = uint8_t;
        static constexpr EngineInitOptions kNone		  = 0b00000;
        static constexpr EngineInitOptions kUseActors	  = 0b00001;
        static constexpr EngineInitOptions kUseInput	  = 0b00010;
        static constexpr EngineInitOptions kUseCollision  = 0b00100;
		static constexpr EngineInitOptions kUseMessaging  = 0b01000;
		static constexpr EngineInitOptions kUseAudio      = 0b11000;
        static constexpr EngineInitOptions kUseAll	      = 0b11111;

    private:
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
        BB_Camera*           m_pMainCamera = nullptr;
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


        [[nodiscard("Manager not being used")]] static BlackBoxManager* Get();
        static BlackBoxManager* NewAndInitEngine(EngineInitOptions options = kUseAll);
        static BlackBoxManager* NewSingleton();
        static void DeleteSingleton();

        int RunEngine();
        int InitEngine(EngineInitOptions options = kUseAll);
    private: // functions
        void HandleSdlEvents();
        void CheckEngineInitialized();
        void UpdateSubsytems();
        BlackBoxManager() = default;
    };
}

