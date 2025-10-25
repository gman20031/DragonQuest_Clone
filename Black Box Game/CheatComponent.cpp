#include "CheatComponent.h"

#include <BlackBoxManager.h>

#include "PlayerMovementComponent.h"
#include "PlayerStatsComponent.h"

using namespace BlackBoxEngine;

static constexpr KeyCode kToggleEncounterKey = KeyCode::kKeyPad_DIVIDE;
static constexpr KeyCode kAddGoldKey = KeyCode::kG;
static constexpr KeyCode kAddXPKey = KeyCode::kE;

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

    // -----------------------------------------------------
    // Add Gold
    // -----------------------------------------------------
    if (auto* pStats = m_pOwner->GetComponent<PlayerStatsComponent>())
    {
        m_inputIds.emplace_back(pInput->SubscribeToKey(
            kAddGoldKey, InputManager::InputType::kKeyDown,
            [pStats]()
            {
                pStats->SetPlayerGold(pStats->GetPlayerGold() + 500);
                BB_LOG(LogType::kMessage, "Cheat: +500 gold (total: %d)", pStats->GetPlayerGold());
            }
        ));

        // -------------------------------------------------
        // Add XP
        // -------------------------------------------------
        m_inputIds.emplace_back(pInput->SubscribeToKey(
            kAddXPKey, InputManager::InputType::kKeyDown,
            [pStats]()
            {
                pStats->SetPlayerExperience(pStats->GetPlayerExperience() + 1000);
                BB_LOG(LogType::kMessage, "Cheat: +1000 XP (total: %d)", pStats->GetPlayerExperience());
            }
        ));
    }
}
