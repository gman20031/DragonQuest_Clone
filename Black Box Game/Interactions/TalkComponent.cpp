#include "TalkComponent.h"
#include <BlackBoxManager.h>
#include "InteractionComponent.h"
#include "../Black Box Game/InventoryComponent.h"

using namespace BlackBoxEngine;

void BaseTalkComponent::OnCollide(BlackBoxEngine::Actor* pOtherActor)
{
    if (!pOtherActor)
        return;

    if (auto* pInteract = pOtherActor->GetComponent<InteractionComponent>())
    {
        pInteract->SetCurrentTalk(this);
        pInteract->SetPlayerActor(pOtherActor);
        

    }
}

void BaseTalkComponent::Save(BlackBoxEngine::XMLElementParser parser)
{
    parser.NewChildVariable("RequiresItem", m_data.requiresItem);
    parser.NewChildVariable("RequiredItemName", m_data.requiredItemName);
}

void BaseTalkComponent::Load(const BlackBoxEngine::XMLElementParser parser)
{
    TalkData data;
    parser.GetChildVariable("RequiresItem", &data.requiresItem);
    parser.GetChildVariable("RequiredItemName", &data.requiredItemName);
    SetTalkData(data);
}

void InnTalkComponent::OnTalkUsed([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{
    BB_LOG(LogType::kMessage, "Innkeeper: 'Welcome traveler! Rest and recover.'");
}

void CastleTalkComponent::OnTalkUsed([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{
    BB_LOG(LogType::kMessage, "Welcome to castle'");
}
