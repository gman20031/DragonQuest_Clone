#include "TalkComponent.h"
#include <BlackBoxManager.h>
#include "InteractionComponent.h"

using namespace BlackBoxEngine;

void TalkComponent::OnTalkUsed([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{
    BB_LOG(LogType::kMessage, "do somehting with inn hahaha");
}

void TalkComponent::OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{
    if (auto* player = pOtherActor->GetComponent<InteractionComponent>())
    {
        player->SetCurrentTalk(this);
        player->SetPlayerActor(pOtherActor);

        BB_LOG(LogType::kMessage, "Player is now on town.");
    }
}
