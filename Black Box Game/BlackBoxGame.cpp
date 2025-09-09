#include "BlackBoxGame.h"

using namespace BlackBoxEngine;

#include <Actors/EngineComponents/SpriteComponent.h>
#include <Resources/ResourceManager.h>

void BlackBoxGame::InitGame()
{
	BB_LOG(LogType::kMessage, "Game starting");

    auto& pActor = m_pEngineManager->m_pActorManager->NewActor();
    auto* pSpriteComponent = pActor->AddComponent<SpriteComponent>();
    pSpriteComponent->SetTexture("../Assets/BlackBoxEngineTitle.png"); // btw, visual studio will give you a preivew of the image if its a proper file path
    pSpriteComponent->SetDimensions(200, 200);

    int width = kDefaultWidth / 2;
    int height = kDefaultHeight / 2;
    pActor->GetComponent<TransformComponent>()->m_position = FVector2(width, height);

    ActorXMLParser::SaveActor(pActor, "TitleActor", "../Assets/TitleActor.xml");
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
    m_pEngineManager->InitEngine(~BlackBoxManager::kUseCollision);
    InitGame();
    m_pEngineManager->RunEngine();
}
