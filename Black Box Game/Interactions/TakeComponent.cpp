#include "TakeComponent.h"

#include "../BlackBoxGame.h"
#include "InteractionComponent.h"

using namespace BlackBoxEngine;

void TakeComponent::OnTakeUsed([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{
	BB_LOG(LogType::kMessage, "take stuff in chest.");
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
