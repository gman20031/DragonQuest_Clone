#include "TakeComponent.h"

#include "../BlackBoxGame.h"
#include "InteractionComponent.h"
#include "../Black Box Game/InventoryComponent.h"
#include "../Black Box Engine/Actors/EngineComponents/SpriteComponent.h"
#include "../GameMessages.h"


using namespace BlackBoxEngine;

TakeComponent::~TakeComponent()
{
    BlackBoxManager::Get()->m_pMessagingManager->RemoveListener( m_messageId );
}

void TakeComponent::OnTakeUsed(Actor* pOtherActor)
{
    BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/44DragonQuest1-OpenTreasure.wav", 0.2f);

	auto* pInventory = pOtherActor->GetComponent<InventoryComponent>();
    auto* pInteraction = pOtherActor->GetComponent<InteractionComponent>();
    if ( !pInteraction || !pInventory )
        return;

    pInteraction->SetCurrentTake( nullptr );
	pInventory->SetHasTablet(true);
	BlackBoxManager::Get()->m_pActorManager->DestroyActor(m_pOwner);
}

void TakeComponent::OnCollide(Actor* pOtherActor)
{
	if (auto* player = pOtherActor->GetComponent<InteractionComponent>())
	{
		player->SetCurrentTake(this);
		player->SetPlayerActor(pOtherActor);

		BB_LOG(LogType::kMessage, "Player is now on chest.");
	}
}

void TakeComponent::Start()
{
    m_messageId = BlackBoxManager::Get()->m_pMessagingManager->RegisterListener( kPlayerMoveStarted,
        [this]( Message& message )
        {
            if ( auto* pInteract = message.pSender->GetComponent<InteractionComponent>() )
                pInteract->SetCurrentTake( nullptr );
        } );
}
