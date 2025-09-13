#include "BlackBoxGame.h"

using namespace BlackBoxEngine;

#include <Actors/EngineComponents/SpriteComponent.h>
#include <Actors/EngineComponents/SimpleTextComponent.h>

void BlackBoxGame::InitGame()
{
	BB_LOG(LogType::kMessage, "Game starting");

    m_pEngineManager->m_pActorManager->LoadActor("../Assets/TitleActor.xml");

    //here i should do ->m_pTileManager->LoadMap();

    const auto& pActor = m_pEngineManager->m_pActorManager->NewActor();
    auto* pTransform = pActor->AddComponent<TransformComponent>();
    auto* pTextComponent = pActor->AddComponent<SimpleTextComponent>();
    pTextComponent->SetFont("../Assets/Fonts/nes-font.ttf" , 24);
    pTextComponent->SetText("Hello, World!");
    pTransform->m_position = { 100,100 };
}

BlackBoxGame::BlackBoxGame()
    : m_pEngineManager(BlackBoxManager::NewSingleton())
{
    m_pEngineManager->CreateWindow(
        "BalatroClone", kDefaultXPos, kDefaultYPos, kDefaultWidth, kDefaultHeight,
        BB_Window::kWindowFlag_Resizable | BB_Window::kWindowFlag_Maximized
    );
    
    BB_LOG(LogType::kMessage, "Window Created");
}

BlackBoxGame::~BlackBoxGame()
{
    if (m_pEngineManager)
        m_pEngineManager->DeleteSingleton();

}

void BlackBoxGame::Launch()
{
    assert(m_pEngineManager);
    m_pEngineManager->InitEngine();
    InitGame();
    m_pEngineManager->RunEngine();
}
