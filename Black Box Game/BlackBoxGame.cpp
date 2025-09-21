#include "BlackBoxGame.h"
#include "../Black Box Engine/Actors/EngineComponents/SpriteComponent.h"

using namespace BlackBoxEngine;

#include <cassert>

#include <System/Delay.h>

void BlackBoxGame::InitGame()
{
	BB_LOG(LogType::kMessage, "Game starting");

    DelayFunction callback = []([[maybe_unused]]void* pData, [[maybe_unused]] uint32_t timerId, [[maybe_unused]] uint32_t milisecondsElapsed)->uint32_t
        {
            BB_LOG(LogType::kMessage, "1 second elapsed");
            return 1000;
        };
    BlackBoxEngine::Delay(1000, callback, nullptr);

    m_pEngineManager->m_pActorManager->LoadLevel("../Assets/Levels/ExampleLevel.xml");
}

BlackBoxGame::BlackBoxGame()
    : m_pEngineManager(BlackBoxManager::NewSingleton())
    , m_pTileActorManager(new TileActorManager)
{
    m_pEngineManager->CreateWindow(
        "Dragon Quest Clone", kDefaultXPos, kDefaultYPos, kDefaultWidth, kDefaultHeight,
        BB_Window::kWindowFlag_Resizable
    );
    
    BB_LOG(LogType::kMessage, "Window Created");
}

BlackBoxGame::~BlackBoxGame()
{
    if (m_pEngineManager)
        m_pEngineManager->DeleteSingleton();
    if (m_pTileActorManager)
        delete m_pTileActorManager;
}

void BlackBoxGame::Launch()
{
    assert(m_pEngineManager);
    m_pEngineManager->InitEngine();
    InitGame();
    m_pTileActorManager->Start();
    m_pEngineManager->RunEngine();
}

BlackBoxGame* BlackBoxGame::NewSingleton()
{
    DeleteSingleton();
    s_pBlackBoxGame = new BlackBoxGame;
    return s_pBlackBoxGame;
}

BlackBoxGame* BlackBoxGame::Get()
{
    assert(s_pBlackBoxGame);
    return s_pBlackBoxGame;
}

void BlackBoxGame::DeleteSingleton()
{
    if (s_pBlackBoxGame)
    {
        delete s_pBlackBoxGame;
        s_pBlackBoxGame = nullptr;
    }
}
