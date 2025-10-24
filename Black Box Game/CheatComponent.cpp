#include "CheatComponent.h"

#include <BlackBoxManager.h>

#include "PlayerMovementComponent.h"

using namespace BlackBoxEngine;

static constexpr KeyCode kToggleEncounterKey = KeyCode::kKeyPad_DIVIDE;

PlayerCheatComponent::~PlayerCheatComponent()
{
    auto* pInput = BlackBoxManager::Get()->m_pInputManager;
    for ( auto id : m_inputIds )
        pInput->UnsubscribeKey( id );
}

void PlayerCheatComponent::Start()
{
    auto* pInput = BlackBoxManager::Get()->m_pInputManager;

    auto* pPlayerMovement = m_pOwner->GetComponent<PlayerMovementComponent>();
    if ( pPlayerMovement )
    {
        m_inputIds.emplace_back( pInput->SubscribeToKey(
            kToggleEncounterKey, InputManager::InputType::kKeyDown,
            [this , pPlayerMovement]() mutable
            {
                pPlayerMovement->ToggleEncounters();
            }
        ) );
    }

}
