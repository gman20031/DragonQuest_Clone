#include "PlayerStatsComponent.h"
#include "PlayerMovementComponent.h"
#include "../Black Box Engine/System/Delay.h"
#include "InteractionComponent.h"
using namespace BlackBoxEngine;

void PlayerStatsComponent::Start()
{
    //Initialize();
}

void PlayerStatsComponent::Update()
{
    auto* playerMove = m_pOwner->GetComponent<PlayerMovementComponent>();
    if (!playerMove) return;

    auto* playerUI = m_pOwner->GetComponent<InteractionComponent>();
    if (!playerUI) return;

    bool isMoving = playerMove->m_isMoving;

    // Show HUD after player stops
    if (!isMoving && !m_hudVisible && !playerUI->m_uiActive && !playerUI->m_isChangingLevel)
    {
        if (playerUI->m_delayedDisplayId == 0)
        {
            playerUI->m_delayedDisplayId = DelayedCallbackManager::AddCallback([this]()
                {
                    auto* playerUI = m_pOwner->GetComponent<InteractionComponent>();
                    if (!playerUI) return;

                    auto* pm = m_pOwner->GetComponent<PlayerMovementComponent>();
                    if (pm && !pm->m_isMoving && !m_hudVisible && !playerUI->m_uiActive && !playerUI->m_isChangingLevel)
                        DisplayHUD();

                    playerUI->m_delayedDisplayId = 0;
                }, 1000);
        }
    }
    // Hide HUD if player moves or UI active
    else if ((isMoving || playerUI->m_uiActive) && m_hudVisible && !playerUI->m_isChangingLevel && !m_forceHUDVisible)
    {
        HideHUD();

        if (playerUI->m_delayedDisplayId != 0)
        {
            DelayedCallbackManager::RemoveCallback(playerUI->m_delayedDisplayId);
            playerUI->m_delayedDisplayId = 0;
        }
    }
}


void PlayerStatsComponent::RefreshHUD()
{
    if (!m_hudVisible)
        return;
    
    // Remove the HUD from screen entirely
    if (m_hudVisible)
        m_hudRoot.RemoveFromScreen();
    
    // Recreate HUD (DisplayHUD) with updated values
    DisplayHUD();

}

std::string PlayerStatsComponent::BuildStatsString() const
{
    return
        "  LV       " + std::to_string(m_playerLevel) + "\n\n" +
        "  HP       " + std::to_string(m_playerHP) + "\n\n" +
        "  MP       " + std::to_string(m_playerMP) + "\n\n" +
        "  G        " + std::to_string(m_playerGold) + "\n\n" +
        "  E        " + std::to_string(m_playerEnergy);
}

void PlayerStatsComponent::Load(const BlackBoxEngine::XMLElementParser parser)
{
    parser.GetChildVariable("Level", &m_playerLevel);
    parser.GetChildVariable("HP", &m_playerHP);
    parser.GetChildVariable("MaxHP", &m_playerMaxHP);
    parser.GetChildVariable("MP", &m_playerMP);
    parser.GetChildVariable("MaxMP", &m_playerMaxMP);
    parser.GetChildVariable("Gold", &m_playerGold);
    parser.GetChildVariable("Energy", &m_playerEnergy);
    parser.GetChildVariable("Strength", &m_playerStrength);
    parser.GetChildVariable("Agility", &m_playerAgility);
}

void PlayerStatsComponent::Save(BlackBoxEngine::XMLElementParser parser)
{
    parser.NewChildVariable("Level", m_playerLevel);
    parser.NewChildVariable("HP", m_playerHP);
    parser.NewChildVariable("MaxHP", m_playerMaxHP);
    parser.NewChildVariable("MP", m_playerMP);
    parser.NewChildVariable("MaxMP", m_playerMaxMP);
    parser.NewChildVariable("Gold", m_playerGold);
    parser.NewChildVariable("Energy", m_playerEnergy);
    parser.NewChildVariable("Strength", m_playerStrength);
    parser.NewChildVariable("Agility", m_playerAgility);
}

void PlayerStatsComponent::DisplayHUD()
{
    if (m_hudVisible) return;
    
    m_hudVisible = true;

    m_hudRoot.RemoveFromScreen();
    
    constexpr BB_FRectangle bgRect{ 10, 10, 50, 70 };
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/StatsInfoBox.png",
        .spriteDimensions = {16, 16},
        .useFullImage = true
    };
    m_hudRoot.AddNode<InterfaceTexture>("HUD_Background", bgRect, bgInfo);
    
    InterfaceText::Paremeters textParams{
        .pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf",
        .textSize = 16,
        .color = ColorPresets::white
    };
    
    // Header
    BB_FRectangle headerRect = bgRect;
    headerRect.h = 15;
    textParams.pText = "     PLAYER";
    m_hudRoot.AddNode<InterfaceText>("HUD_Header", headerRect, textParams);
    
    //auto* playerStats = m_pOwner->GetComponent<PlayerStatsComponent>();
    // Stats
    BB_FRectangle statsRect = bgRect;
    statsRect.y += 12;
    std::string stats =
        "  LV       " + std::to_string(m_playerLevel) + "\n\n" +
        "  HP       " + std::to_string(m_playerHP) + "\n\n" +
        "  MP       " + std::to_string(m_playerMP) + "\n\n" +
        "  G        " + std::to_string(m_playerGold) + "\n\n" +
        "  E        " + std::to_string(m_playerEnergy);
    textParams.pText = stats.c_str();
    m_hudStatsText = m_hudRoot.AddNode<InterfaceText>("HUD_Stats", statsRect, textParams);
    
    m_hudRoot.AddToScreen();

}

void PlayerStatsComponent::HideHUD()
{
    if (!m_hudVisible) return;
    m_hudRoot.RemoveFromScreen();
    m_hudVisible = false;
}

