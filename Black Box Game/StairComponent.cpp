#include "StairComponent.h"

#include <chrono>
#include <BlackBoxManager.h>
#include <Graphics/ScreenFader.h>
#include <Actors/EngineComponents/TransformComponent.h>
#include <System/Delay.h>

#include "BlackBoxGame.h"
#include "InteractionComponent.h"

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

    auto* pManager = BlackBoxManager::Get();
    auto currentPos = pTransform->m_position;
    pTransform->m_position = {16, 16};
    pManager->m_pActorManager->SaveActor( pOtherActor, "PlayerActor", "../Assets/Actors/PlayerActor.xml" );
    pTransform->m_position = currentPos;

    // stop all input.
    pManager->m_pInputManager->StopAllInput();
    ScreenFader::FadeToBlack( 1.f );

    auto delayFunc = [pManager]()
        {
            pManager->m_pInputManager->SwapInputToGame();
            pManager->m_pActorManager->LoadLevel( "../Assets/Levels/Cave1Level.xml" );
            pManager->m_pActorManager->Start();
            pManager->m_pInputManager->ResumeInput();

            ScreenFader::FadeIn( 1.f );
            return 0;
        };
    Delay( std::chrono::seconds( 1 ), delayFunc );

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
