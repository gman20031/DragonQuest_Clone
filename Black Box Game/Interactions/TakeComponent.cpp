#include "TakeComponent.h"

#include "../BlackBoxGame.h"
#include "InteractionComponent.h"
#include "../Black Box Game/InventoryComponent.h"
#include "../Black Box Engine/Actors/EngineComponents/SpriteComponent.h"


using namespace BlackBoxEngine;

void TakeComponent::OnTakeUsed([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{
	auto* pInventory = pOtherActor->GetComponent<InventoryComponent>();

	pInventory->SetHasTablet(true);
	BlackBoxManager::Get()->m_pActorManager->DestroyActor(m_pOwner);

}

void TakeComponent::OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{
	if (auto* player = pOtherActor->GetComponent<InteractionComponent>())
	{
		player->SetCurrentTake(this);
		player->SetPlayerActor(pOtherActor);

		BB_LOG(LogType::kMessage, "Player is now on chest.");
	}
}
