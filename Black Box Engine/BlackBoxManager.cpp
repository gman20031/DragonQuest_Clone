#include "BlackBoxManager.h"

#include <SDL3_mixer/SDL_mixer.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <algorithm>

#include "Graphics/ScreenFader.h"
#include "System/SimpleTimingSystem.h"
#include "System/Debugging.h"
#include "Actors/ActorManager.h"
#include "System/Delay.h"

namespace BlackBoxEngine
{
    BlackBoxManager* BlackBoxManager::m_pSingletonManager{ nullptr };

#ifdef _DEBUG
    void BlackBoxManager::CheckEngineInitialized()
    {
        if (m_engineOptions & kActorSystem && !m_pActorManager)
            BB_LOG(LogType::kFailure, "Engine has no actor Manager");
        if (m_engineOptions & kInput && !m_pInputManager)
			BB_LOG(LogType::kFailure, "Engine has no input Manager");
		if (m_engineOptions & kMessaging && !m_pMessagingManager)
			BB_LOG(LogType::kFailure, "Engine has no message Manager");
		if (m_engineOptions & kCollision && !m_pCollisionManager)
            BB_LOG(LogType::kFailure, "Engine has no collision Manager");
        if (!m_pWindow)
            BB_LOG(LogType::kFailure, "Engine has no window");
    }

#endif // _DEBUG

    BlackBoxManager::~BlackBoxManager()
    {
        if (m_pActorManager)
            delete m_pActorManager;
        if (m_pInputManager)
            delete m_pInputManager;
        if (m_pMessagingManager)
            delete m_pMessagingManager;
        if (m_pCollisionManager)
            delete m_pCollisionManager;
        SDL_QuitSubSystem( SDL_INIT_AUDIO );
    }

    void BlackBoxManager::HandleSdlEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_KEY_DOWN:
                m_pInputManager->AddKeyDown((KeyCode)event.key.scancode);
                break;
            case SDL_EVENT_KEY_UP:
                m_pInputManager->RemoveKeyDown((KeyCode)event.key.scancode);
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                m_pWindow->NotifyWindowResized(event.window.data1, event.window.data2);
                break;
            case SDL_EVENT_QUIT:
                m_keepRunning = false;
                break;
            }
        }
    }

    void BlackBoxManager::CreateWindow( const char* pWindowTitle, int xPos, int yPos, int width, int height, BB_Window::WindowFlags flags)
    {
        m_pWindow = std::make_unique<BB_Window>(pWindowTitle, xPos, yPos, width, height, flags);
    }

	bool BlackBoxManager::IsSystemEnabled(EngineInitOptions option) const
	{
		return m_engineOptions & option;
	}

    void BlackBoxManager::SetGameUpdate( GameUpdateFunc&& callback )
    {
        m_gameUpdateFunction = callback;
    }

    void BlackBoxManager::UpdateSubsytems()
    {
        std::unique_lock lock( m_engineMutex );
        /// Update global Systems
        HandleSdlEvents();

        DelayedCallbackManager::Update( m_deltaTime * 1'000);

        if (IsSystemEnabled(kInput) ) 
            m_pInputManager->Update();

        if ( m_gameUpdateFunction )
            m_gameUpdateFunction();
        if (IsSystemEnabled(kActorSystem) ) 
        {
            m_pActorManager->Update();
            m_pActorManager->Render();
        }
#if _DEBUG
        if (m_engineOptions & kCollision) 
            m_pCollisionManager->DebugDraw();
#endif
        if (IsSystemEnabled(kMessaging)) 
            m_pMessagingManager->SendQueuedMessages();
        m_pInterfaceManager->Render(m_pWindow->GetRenderer());

        if ( ScreenFader::m_fading )
        {
            ScreenFader::Update( static_cast<float>(m_deltaTime) );
            ScreenFader::Render();
        }
    }

    int BlackBoxManager::RunEngine()
    {
        if constexpr (kDebug)
        {
            BB_LOG(LogType::kMessage, "Game started in debug mode");
            CheckEngineInitialized();
        }

        SimpleTimer timer;
        timer.StartTimer();

        while (m_keepRunning)
        {
            m_deltaTime = timer.GetDeltaTime();
            if ( m_deltaTime > .250 )
                m_deltaTime = .250;
            timer.StartTimer();

            auto* pRenderer = m_pWindow->GetRenderer();
            pRenderer->Clear();

            UpdateSubsytems();

            pRenderer->Present();
        }

        return 0;
    }

    int BlackBoxManager::InitEngine(EngineInitOptions options)
    {
		m_engineOptions = options;

        // system inits
		if(options & kMessaging)
			m_pMessagingManager = new MessagingManager;
		if(options & kInput)
			m_pInputManager = new InputManager;
		if(options & kActorSystem)
			m_pActorManager = new ActorManager;
        if (options & kAudio)
            m_pAudioManager = new AudioManager;
		if(options & kCollision)
		{
			m_pCollisionManager = new CollisionManager(-100, -100,
				1408,
				1232);
		}

        // other inits
        TTF_Init();
        SDL_Init(SDL_INIT_VIDEO);
        m_pMainCamera = new BB_Camera;
        m_pInterfaceManager = new InterfaceManager;

        // start the window
        m_keepRunning = true;
        assert(m_pWindow);
        if (m_pWindow->StartWindow() != 0)
            m_keepRunning = false;

        return 0;
    }

    BlackBoxManager* BlackBoxManager::NewAndInitEngine(EngineInitOptions options)
    {
        NewSingleton();
        m_pSingletonManager->InitEngine(options);
        return m_pSingletonManager;
    }

    BlackBoxManager* BlackBoxManager::Get()
    {
        assert(m_pSingletonManager);
        return m_pSingletonManager;
    }

    BlackBoxManager* BlackBoxManager::NewSingleton()
    {
        DeleteSingleton();
        m_pSingletonManager = new BlackBoxManager;
        return m_pSingletonManager;
    }

    void BlackBoxManager::DeleteSingleton()
    {
        if (m_pSingletonManager)
        {
            delete m_pSingletonManager;
            m_pSingletonManager = nullptr;
        }
    }
}
