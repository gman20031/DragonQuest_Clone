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

	auto* pSprite = m_pOwner->GetComponent<SpriteComponent>();
	pSprite->SetTextureOffset({ 51, 17, 16, 16 });
	pSprite->SetTexture(".. /Assets/Tiles/FullTileSheet.png");
	

	m_hasTablet = true;
}

void TakeComponent::OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{
	if (m_hasTablet == true)
		return;
	else
	{
		if (auto* player = pOtherActor->GetComponent<InteractionComponent>())
		{
			player->SetCurrentTake(this);
			player->SetPlayerActor(pOtherActor);

			BB_LOG(LogType::kMessage, "Player is now on chest.");
		}
	}
}
