#include "PlayerStatsComponent.h"

#include <BlackBoxManager.h>
#include <Actors/Actor.h>
#include <System/Delay.h>
#include <Interface/InterfaceTexture.h>
#include <Graphics/Text Rendering/Text.h>

#include "GameMessages.h"

using namespace BlackBoxEngine;

PlayerStatsComponent::~PlayerStatsComponent()
{
    auto* pMessager = BlackBoxManager::Get()->m_pMessagingManager;
    for ( auto id : m_messageIds )
        pMessager->RemoveListener( id );
}

void PlayerStatsComponent::SetPlayerLevel( int value )
{
    m_playerLevel = value;
    RefreshHUD();
}

void PlayerStatsComponent::SetPlayerHP( int value )
{
    m_playerHP = value;
    RefreshHUD();
}

void PlayerStatsComponent::SetPlayerMP( int value )
{
    m_playerMP = value;
    RefreshHUD();
}

void PlayerStatsComponent::SetPlayerGold( int value )
{
    m_playerGold = value;
    RefreshHUD();
}

void PlayerStatsComponent::SetPlayerExperience( int value )
{
    m_playerEXP = value;
    RefreshHUD();
}

void PlayerStatsComponent::CalculatePlayerXP()
{

}

void PlayerStatsComponent::Update()
{
    if ( !m_pTransform || m_changingLevel )
        return;

    bool moving = m_pTransform->m_position != m_pTransform->m_prevPosition;

    if ( moving && m_callbackActive )
        DelayedCallbackManager::RemoveCallback( m_callbackId );

    // Show HUD after player stops
    if ( moving && m_hudVisible )
    {
        HideHUD();
        return;
    }
    else
    {
        //THE DELAY DO NOT WORK ANYMORE
        m_callbackActive = true;
        m_callbackId = DelayedCallbackManager::AddCallback([this]()
        {
            DisplayHUD();
        }, 1000);
    }
}

void PlayerStatsComponent::Start()
{
    m_pTransform = m_pOwner->GetComponent<TransformComponent>();
    if ( !m_pTransform )
    {
        BB_LOG( LogType::kWarning, "attached actor does not have a transform, will never display stats" );
        return;
    }

    CreateHud();

    m_messageIds.emplace_back( BlackBoxManager::Get()->m_pMessagingManager->RegisterListener
    ( 
        kLevelChanging, [this]( [[maybe_unused]] Message& ) { OnLevelChange(); }
    ));
}

std::string PlayerStatsComponent::BuildStatsString() const
{
    return
        "LV       " + std::to_string(m_playerLevel) + "\n\n" +
        "HP       " + std::to_string(m_playerHP)    + "\n\n" +
        "MP       " + std::to_string(m_playerMP)    + "\n\n" +
        "G        " + std::to_string(m_playerGold)  + "\n\n" +
        "E        " + std::to_string(m_playerEXP);
}

void PlayerStatsComponent::OnLevelChange()
{
    m_changingLevel = true;
    m_hudRoot.SetCursorTarget( nullptr );
    m_hudRoot.RemoveFromScreen();
}

void PlayerStatsComponent::Load(const BlackBoxEngine::XMLElementParser parser)
{
    parser.GetChildVariable("Level", &m_playerLevel);
    parser.GetChildVariable("HP", &m_playerHP);
    parser.GetChildVariable("MaxHP", &m_playerMaxHP);
    parser.GetChildVariable("MP", &m_playerMP);
    parser.GetChildVariable("MaxMP", &m_playerMaxMP);
    parser.GetChildVariable("Gold", &m_playerGold);
    parser.GetChildVariable("XP", &m_playerEXP);
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
    parser.NewChildVariable("XP", m_playerEXP);
    parser.NewChildVariable("Strength", m_playerStrength);
    parser.NewChildVariable("Agility", m_playerAgility);
}

void PlayerStatsComponent::CreateHud()
{
    static constexpr float kTileSize         = 16;
    static constexpr float kHudPosX          = 0.5 * kTileSize;
    static constexpr float kHudPosY          = 1.5 * kTileSize;
    static constexpr float kBackgroundWidth  = 5 * kTileSize;
    static constexpr float kBackgroundHeight = 7 * kTileSize;
    static constexpr float kTextSize         = 28;
    static constexpr float kNameTextSize     = kTextSize * 2 / 3;

    static constexpr float kStatsXOff = 0.5 * kTileSize;
    static constexpr float kStatsYOff = 1 * kTileSize;

    m_hudRoot.GetRoot()->SetOffset( kHudPosX, kHudPosY );

    constexpr BB_FRectangle bgRect{0, 0, kBackgroundWidth, kBackgroundHeight};
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/StatsInfoBox.png",
        .useFullImage = true
    };
    auto* pBackgroundTexture = m_hudRoot.AddNode<InterfaceTexture>( "HUD_Background", bgRect, bgInfo );

    InterfaceText::Paremeters textParams{
        .pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf",
        .textSize = kTextSize,
        .color = ColorPresets::white
    };

    // PlayerNameTextElement
    textParams.textSize = kNameTextSize;
    BB_FRectangle nameRect = {kBackgroundWidth / 4, 0, kBackgroundWidth, 28};
    textParams.pText = "PLAYER";
    pBackgroundTexture->MakeChildNode<InterfaceText>( "HUD_Header", nameRect, textParams );

    // Stats
    textParams.textSize = kTextSize;
    BB_FRectangle statsRect = {kStatsXOff, kStatsYOff, kBackgroundWidth, kBackgroundHeight};
    std::string stats = BuildStatsString();
    textParams.pText = stats.c_str();
    m_hudStatsText = pBackgroundTexture->MakeChildNode<InterfaceText>( "HUD_Stats", statsRect, textParams );

}

void PlayerStatsComponent::DisplayHUD()
{
    if (m_hudVisible) 
        return;
    
    m_hudRoot.AddToScreen();
    m_hudVisible = true;
}

void PlayerStatsComponent::HideHUD()
{
    if (!m_hudVisible) 
        return;

    m_hudRoot.RemoveFromScreen();
    m_hudVisible = false;
}

void PlayerStatsComponent::RefreshHUD()
{
    if (!m_hudVisible)
        return;

    if (m_hudStatsText)
    {
        auto textPtr = m_hudStatsText->GetText();
        if (textPtr)
        {
            std::string stats = BuildStatsString();
            textPtr->SetString(stats.c_str(), stats.size()); // update the stats live
        }
    }
}