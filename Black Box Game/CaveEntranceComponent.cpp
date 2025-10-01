#include "CaveEntranceComponent.h"
#include "../Black Box Engine/BlackBoxManager.h"
#include "BlackBoxGame.h"


using namespace BlackBoxEngine;

void CaveEntranceComponent::OnInteract()
{
	BB_LOG(LogType::kMessage, "Entering the cave...");

	BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();
	BlackBoxManager::Get()->m_pActorManager->LoadLevel("../Assets/Levels/Cave1Level.xml");
    BlackBoxManager::Get()->m_pActorManager->Start();

}

void CaveEntranceComponent::Start()
{
}

void CaveEntranceComponent::Update()
{
}

void CaveEntranceComponent::Render()
{
}

void CaveEntranceComponent::OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{
}

void CaveEntranceComponent::Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser)
{
}

void CaveEntranceComponent::Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser)
{
}
