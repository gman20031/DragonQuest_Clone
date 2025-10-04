#include "StairComponent.h"
#include "../Black Box Engine/BlackBoxManager.h"
#include "BlackBoxGame.h"
#include "InteractionComponent.h"
#include "../Black Box Engine/Actors/EngineComponents/TransformComponent.h"


using namespace BlackBoxEngine;

void CaveEntranceComponent::OnStairUsed(Actor* pOtherActor)
{
	BB_LOG(LogType::kMessage, "Entering the cave...");

    // check if player
    auto* pInteractComponent = pOtherActor->GetComponent<InteractionComponent>();
    if ( !pInteractComponent )
        return;
    // player should have transform
    auto* pTransform = pOtherActor->GetComponent<TransformComponent>();
    if ( !pTransform )
        return;

    pTransform->m_position = {16, 16};
    BlackBoxManager::Get()->m_pActorManager->SaveActor( pOtherActor, "PlayerActor", "../Assets/Actors/PlayerActor.xml" );

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
	//auto* pComponent = pOtherActor->GetComponent<InteractionComponent>();

	if (auto* player = pOtherActor->GetComponent<InteractionComponent>())
	{
		player->SetCurrentStair(this);
		player->SetPlayerActor(pOtherActor);
		//if(player->GetDidMove())
		//{
		//	//check if did press stair
		//	pOtherActor->GetComponent<TransformComponent>()->m_position.x = 16;
		//	pOtherActor->GetComponent<TransformComponent>()->m_position.y = 16;
		//}
		//BlackBoxManager::Get()->m_pActorManager->SaveActor(pOtherActor, "PlayerActor");

		BB_LOG(LogType::kMessage, "Player is now on cave entrance.");
	}

}

void CaveEntranceComponent::Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser)
{
}

void CaveEntranceComponent::Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser)
{
}

void StairComponent::Start()
{
}

void StairComponent::Update()
{
}

void StairComponent::Render()
{
}

void StairComponent::OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{
}

void StairComponent::Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser)
{
}

void StairComponent::Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser)
{
}
