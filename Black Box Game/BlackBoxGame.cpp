#include "BlackBoxGame.h"

using namespace BlackBoxEngine;

#include <Actors/EngineComponents/SpriteComponent.h>
#include <Actors/EngineComponents/SimpleTextComponent.h>
#include "ScreenCentered.h"

void BlackBoxGame::InitGame()
{
	BB_LOG(LogType::kMessage, "Game starting");

    m_pEngineManager->m_pActorManager->LoadLevel("../Assets/Levels/ExampleLevel.xml");
}

BlackBoxGame::BlackBoxGame()
    : m_pEngineManager(BlackBoxManager::NewSingleton())
{
    m_pEngineManager->CreateWindow(
        "BalatroClone", kDefaultXPos, kDefaultYPos, kDefaultWidth, kDefaultHeight,
        BB_Window::kWindowFlag_Resizable
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
