#include "TalkComponent.h"
#include <BlackBoxManager.h>
#include "InteractionComponent.h"

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

    BB_LOG(LogType::kMessage, "Guard: 'Halt! You need the Royal Pass.'");
    //if (auto* inventory = pOtherActor->GetComponent<InventoryComponent>())
    //{
    //    if (inventory->HasItem("RoyalPass"))
    //    {
    //        BB_LOG(LogType::kMessage, "Guard: 'You may enter, my lord.'");
    //    }
    //    else
    //    {
    //        BB_LOG(LogType::kMessage, "Guard: 'Halt! You need the Royal Pass.'");
    //    }
    //}
    //else
    //{
    //    BB_LOG(LogType::kMessage, "Guard: 'You shouldn't be here.'");
    //}
}
