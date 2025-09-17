#include "BlackBoxGame.h"

using namespace BlackBoxEngine;

#include <cassert>

#include "TileSystem/TileInfoComponent.h"
#include "TileSystem/TileMapComponent.h"
#include "../Black Box Engine/Actors/EngineComponents/SpriteComponent.h"
#include "PlayerMovementComponent.h"
#include "../Black Box Engine/Actors/EngineComponents/MoverComponent.h"

void BlackBoxGame::InitGame()
{
	BB_LOG(LogType::kMessage, "Game starting");

    m_pEngineManager->m_pActorManager->LoadLevel("../Assets/Levels/ExampleLevel.xml");

   // [[maybe_unused]] auto& pPlayerActor = m_pEngineManager->m_pActorManager->LoadActor("../Assets/Actors/PlayerActor.xml");
    
    //[[maybe_unused]] const auto& pPlayerActor = m_pEngineManager->m_pActorManager->NewActor();
    //auto* pComponent = pPlayerActor->AddComponent<SpriteComponent>();
    //pComponent->SetTexture("../Assets/Sprites/Player/Player_1.png");
    //pComponent->SetDimensions(34, 34);
    //
    //pPlayerActor->AddComponent<MoverComponent>();
    //pPlayerActor->AddComponent<PlayerMovementComponent>();
    //
    //m_pEngineManager->m_pActorManager->SaveActor(pPlayerActor, "PlayerActor");
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
