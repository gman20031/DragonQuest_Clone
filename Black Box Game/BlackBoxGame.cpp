#include "BlackBoxGame.h"

using namespace BlackBoxEngine;

#include "LevelFilePaths.h"

#include <cassert>

void BlackBoxGame::InitGame()
{
	BB_LOG(LogType::kMessage, "Game starting");
    m_pEngineManager->GetWindow()->SetWindowIcon("../Assets/Sprites/DragonQuestIcon.png");

    m_pEngineManager->m_pActorManager->LoadLevel(kOverworldLevel);
    m_gameStarter.Start();
    m_pEngineManager->SetGameUpdate( [this]() { GameUpdate(); } );
}

BlackBoxGame::BlackBoxGame()
    : m_pEngineManager(BlackBoxManager::NewSingleton())
    , m_pTileActorManager(new TileActorManager)
    , m_gameStarter("../Assets/Actors/StartingPlayerInformation.xml")
{
    m_pEngineManager->CreateWindow(
        "Dragon Quest Clone", kDefaultXPos, kDefaultYPos, kDefaultWidth, kDefaultHeight, 0
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
    m_pEngineManager->RunEngine();
}

void BlackBoxGame::GameUpdate()
{
    m_pTileActorManager->Update();
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
