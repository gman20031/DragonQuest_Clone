#include "EncounterComponent.h"

#include <BlackBoxManager.h>
#include <Actors/ActorManager.h>
#include <Actors/EngineComponents/SpriteComponent.h>
#include <Actors/EngineComponents/TransformComponent.h>
#include <Actors/EngineComponents/MoverComponent.h>
#include <Graphics/ScreenFader.h>
#include <System/Delay.h>
#include <format>

#include <Interface/InterfaceButton.h>
#include <Interface/InterfaceText.h>

#include "../BlackBoxGame.h"
#include "../Interactions/InteractionComponent.h"
#include "../GameMessages.h"
#include "EncounterHandler.h"
#include "../Black Box Game/InventoryComponent.h"

static constexpr int kTileSize = 16;

static constexpr float kMessageBoxStartX = 2 * kTileSize;
static constexpr float kMessageBoxStartY = 9.5 * kTileSize;
static constexpr float kMessageBoxWidth =  12 * kTileSize;
static constexpr float kMessageBoxHeight = 5 * kTileSize;
static constexpr float kStandardUITextSize = 28.f;
static constexpr KeyCode kSelectkey = KeyCode::kX;
static constexpr KeyCode kBackKey = KeyCode::kZ;

static constexpr int kCommandWindowStartX   = 6 * kTileSize;
static constexpr int kCommandWindowStartY   = static_cast<int>(0.5 * kTileSize);
static constexpr int kCommandWindowWidth    = 8 * kTileSize;
static constexpr int kCommandWindowHeight   = static_cast<int>(3.5 * kTileSize);

static constexpr float kEnemyXoffset = -kTileSize / 2;
static constexpr float kEnemyYoffset = kCommandWindowHeight;
static constexpr float kEnemyWidth  = 7 * kTileSize;
static constexpr float kEnemyHeight = 5.5 * kTileSize;

using namespace BlackBoxEngine;

EncounterComponent::EncounterComponent( BlackBoxEngine::Actor* pOwner )
    : Component( pOwner )
{
    m_randomMachine.Reset( std::time( 0 ) );

    constexpr BB_FRectangle bgRect{kMessageBoxStartX, kMessageBoxStartY, kMessageBoxWidth, kMessageBoxHeight};
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/BottomTextBox.png",
        .useFullImage = true
    };
    m_pMessageBackground = m_combatRoot.AddNode<InterfaceTexture>( "ActionMessage_BG", bgRect, bgInfo );

    m_combatRoot.SetInterfaceKeys( UserInterface::InterfaceKeys{
            .select = kSelectkey
    } );

    CreateItemBox();
}

EncounterComponent::~EncounterComponent()
{
    m_combatRoot.RemoveFromScreen();
    BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();
    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage( kMessageUIClosed, m_pOwner );
}

void EncounterComponent::StartEncounter(Actor* pOtherActor)
{
    BB_LOG(LogType::kMessage, "Enemy '%s' appeared! HP=%d", m_name.c_str(), m_hp);
    
    if ( !pOtherActor->GetComponent<PlayerStatsComponent>() )
    {
        BB_LOG( LogType::kError, "Actor passed into startEncounter, did not have player stat component" );
        return;
    }

    auto* pMover = pOtherActor->GetComponent<MoverComponent>();
    if ( pMover )
    {
        pMover->StopVelocity();
        pMover->Disable();
    }

    BlackBoxManager::Get()->m_pInputManager->SwapInputTargetToInterface( &m_combatRoot );
    BlackBoxManager::Get()->m_pAudioManager->SetMusicTrack( "../Assets/Audio/24DragonQuest1-MonsterBattle.wav" );
    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage( kMessageUIOpen, m_pOwner );

    m_combatRoot.AddToScreen();

    m_pPlayer = pOtherActor;
    m_playerDead = false;
    StartCombatUI();
    ShowActionMessage( std::format( "Enemy {} appeared! HP: {}\n\nCommand? ", m_name.c_str(), m_hp ) );
}

void EncounterComponent::EndEncounter()
{
    BlackBoxManager::Get()->m_pAudioManager->SetMusicTrack( "../Assets/Audio/05DragonQuest1-KingdomofAlefgard.wav" );

    InterfaceButton::ButtonParams btnParams{
        .usable = true,
        .color = ColorValue{ 0, 0, 0, 0 },
        .targetedColor = ColorValue{ 0, 0, 0, 0 },
        .interactColor = ColorValue{ 0, 0, 0, 0 },
        .callbackFunction = [this](){BlackBoxManager::Get()->m_pActorManager->DestroyActor( m_pOwner );}
    };

    auto* pButton = m_pMessageBackground->MakeChildNode<InterfaceButton>( "quitCombat", BB_FRectangle{0,0,0,0},btnParams);
    m_combatRoot.SetCursorTarget( pButton );
    m_combatRoot.GetHighlight()->SetParameters( {.mode = 0} );

    auto* pMover = m_pPlayer->GetComponent<MoverComponent>();
    if ( pMover )
        pMover->Enable();

    DelayedCallbackManager::AddCallback( []() { BlackBoxManager::Get()->m_pInputManager->ResumeInput(); }, 1000 );

    BB_LOG(LogType::kMessage, "Encounter ended.");
}

void EncounterComponent::PlayerDies()
{
    BlackBoxManager::Get()->m_pAudioManager->RemoveMusicTrack();
    BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/20DragonQuest1-ThouHastDied.wav");

    m_playerDead = true;

    ShowActionMessage( "You are defeated!" );
    BlackBoxManager::Get()->m_pInputManager->StopAllInput();
    auto delayFunc = [this]()
        { 
            EndEncounter();
            BlackBoxManager::Get()->m_pActorManager->DestroyActor( m_pOwner );
            RespawnPlayer();
        };
    DelayedCallbackManager::AddCallback( delayFunc, std::chrono::milliseconds( 1500 ) );
}

void EncounterComponent::Start()
{

}

//to change depending on the enemy
void EncounterComponent::Load( const XMLElementParser parser )
{
    parser.GetChildVariable( "Name", &m_name );
    parser.GetChildVariable( "HP", &m_hp );
    parser.GetChildVariable( "Attack", &m_attack );
    parser.GetChildVariable( "Defense", &m_defense );
    parser.GetChildVariable( "XPReward", &m_xpReward );
    parser.GetChildVariable( "GoldReward", &m_goldReward );
    parser.GetChildVariable( "Agility", &m_agility );
    parser.GetChildVariable( "Dodge", &m_dodgeChance );
    parser.GetChildVariable( "SpriteFile", &m_spriteFile );
}

void EncounterComponent::Save( XMLElementParser parser )
{
    parser.NewChildVariable( "Name", m_name );
    parser.NewChildVariable( "HP", m_hp );
    parser.NewChildVariable( "Attack", m_attack );
    parser.NewChildVariable( "Defense", m_defense );
    parser.NewChildVariable( "XPReward", m_xpReward );
    parser.NewChildVariable( "GoldReward", m_goldReward );
    parser.NewChildVariable( "Agility", m_agility );
    parser.NewChildVariable( "Dodge", m_dodgeChance );
    parser.NewChildVariable( "SpriteFile", m_spriteFile );
}

void EncounterComponent::EnemyTakeTurn()
{
    BlackBoxManager::Get()->m_pInputManager->StopAllInput();
    DelayedCallbackManager::AddCallback( 
        [this]() { DoEnemyAction(); }
        , std::chrono::milliseconds( 1000 ) 
    );
}

void EncounterComponent::DoEnemyAction()
{
    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pStats) 
        return;

    float roll = m_randomMachine.GetRandomInRange(1.0f);
    if (m_name == "BlueSlime" || m_name == "RedSlime")
    {
        BasicAttack();
    }
    else if (m_name == "Drakee")
    {
        if (roll < 0.2f)
            ShowActionMessage("The Drakee misses!");
        else
            BasicAttack();
    }
    else if (m_name == "Ghost")
    {
        if (roll < 0.25f)
            ShowActionMessage("The Ghost fades away, dodging your strike!");
        else
            BasicAttack();
    }
    else if (m_name == "Magician")
    {
        if (roll < 0.5f)
            CastSpell("Hurt");
        else
            BasicAttack();
    }
    else
        BasicAttack();

    if ( !m_playerDead )
        BlackBoxManager::Get()->m_pInputManager->ResumeInput();
}

void EncounterComponent::PlayerAttack()
{
    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pStats) 
        return;

    int roll = m_randomMachine.GetRandomInRange(64);
    if (roll < m_dodgeChance )
    {
        ShowActionMessage(std::format("The {} dodges your attack!", m_name));
        EnemyTakeTurn();
        return;
    }

    int playerAtk = pStats->GetPlayerStrength();
    int damage = std::max(1, playerAtk - m_defense);
    m_hp -= damage;

    ShowActionMessage(std::format("You hit the {} for {} damage!", m_name, damage));

    if (m_hp <= 0)
    {
        pStats->SetPlayerExperience(pStats->GetPlayerExperience() + m_xpReward);
        pStats->SetPlayerGold(pStats->GetPlayerGold() + m_goldReward);
        ShowActionMessage( std::format("The {} is defeated!", m_name) );
        EndEncounter();
    }
    else
    {
        EnemyTakeTurn();
        return;
    }
}

void EncounterComponent::TryToFlee()
{
    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pStats) return;

    // --- Calculate dynamic flee chance based on agility difference ---
    // If player Agility is higher than the monster’s, easier to flee; lower makes it harder.
    // The formula yields roughly:
    //   - 0.9f max chance (easy escape)
    //   - 0.1f min chance (very difficult)
    float agilityDiff = static_cast<float>(pStats->GetPlayerAgility() - m_agility);
    float fleeChance = std::clamp(0.5f + agilityDiff * 0.02f, 0.1f, 0.9f);

    if (m_name == "Ghost")
        fleeChance *= 0.8f; // Slightly harder to escape Ghosts (eerie persistence)
    else if (m_name == "Magician")
        fleeChance *= 0.9f; // Spellians may try to trap you with Spell

    // --- Roll the outcome ---
    if ( m_randomMachine.GetRandomInRange( 1.0f ) < fleeChance)
    {
        BlackBoxManager::Get()->m_pInputManager->StopAllInput();
        ShowActionMessage("You successfully escaped!");
        EndEncounter();
    }
    else
    {
        ShowActionMessage("You cannot escape!");
        EnemyTakeTurn();
    }
}


void EncounterComponent::BasicAttack()
{
    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pStats) return;

    int defense = pStats->GetPlayerStrength();  // use real defense
    int damage = std::max(1, m_attack - defense);

    int currentHP = pStats->GetPlayerHP();
    pStats->SetPlayerHP(currentHP - damage);

    ShowActionMessage(std::format("The {} attacks! You take {} damage!", m_name.c_str(), damage));

    // Check if player died
    if (pStats->GetPlayerHP() <= 0)
        PlayerDies();
}

void EncounterComponent::CastSpell( const std::string& spellName)
{
    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pStats) return;

    if (spellName == "Hurt")
    {
        int damage = m_randomMachine.GetRandomInRange(8,13); // 8–13 damage
        int currentHP = pStats->GetPlayerHP();
        pStats->SetPlayerHP(std::max(0, currentHP - damage));

        ShowActionMessage(std::format("The {} casts Hurt! You take {} damage!", m_name.c_str(), damage));

        if (pStats->GetPlayerHP() <= 0)
            PlayerDies();
    }
}

void EncounterComponent::StartCombatUI()
{
    // Command Panel
    BB_FRectangle panelRect{
        kCommandWindowStartX, kCommandWindowStartY, kCommandWindowWidth, kCommandWindowHeight
    };
    InterfaceTexture::TextureInfo panelInfo{};
    panelInfo.pTextureFile = "../Assets/UI/SelectionBox.png";
    panelInfo.useFullImage = true;
    m_pCommandBackground = m_combatRoot.AddNode<InterfaceTexture>(
        "CommandPanelBG", panelRect, panelInfo );

    // --- Enemy sprite ---
    BB_FRectangle bgRect{ kEnemyXoffset, kEnemyYoffset, kEnemyWidth, kEnemyHeight};
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = m_spriteFile.c_str(),
        .useFullImage = true,
    };
    m_pCommandBackground->MakeChildNode<InterfaceTexture>( "Combat_BG", bgRect, bgInfo );

    // highlighter
    auto* highlighter = m_combatRoot.GetHighlight();
    highlighter->SetParameters( {
        .mode = InterfaceHighlighter::kModeIcon,
        .pSpriteFile = "../Assets/UI/Icons/IconSpriteFile.xml",
        .iconOffset{-5, 0},
        .iconSize{4, 7}
        } );
    CreateCommandButtons( m_pCommandBackground );
}

void EncounterComponent::CreateCommandButtons( InterfaceTexture* pBackground )
{
    using enum Direction;
    static constexpr float kButtonWidth = 50.f;
    static constexpr float kButtonHeight = 7.f;
    static constexpr float kXStart = 16.f;
    static constexpr float kYStart = 16.f;
    static constexpr float kYPad = 16.f;
    static constexpr float kXPad = 6.f;
    static constexpr int kButtonCount = 4;
    static constexpr int kButtonGridWidth = 2;
    static constexpr int kButtonGridHeight = kButtonCount / kButtonGridWidth;

    auto gridX = []( int i ) { return i % kButtonGridWidth; };
    auto gridY = []( int i ) { return i / kButtonGridWidth; };
    auto gridIndex = []( int x, int y ) { return x + (y * kButtonGridHeight); };

    // --- Combat Menu Buttons ---
    static constexpr const char* actions[kButtonCount] = {"Fight", "Spell", "Run", "Item"};
    InterfaceButton::ButtonParams btnParams{
        .usable = true,
        .color = ColorValue{ 0, 0, 0, 0 },
        .targetedColor = ColorValue{ 0, 0, 0, 0 },
        .interactColor = ColorValue{ 0, 0, 0, 0 },
    };
    // Text labels
    InterfaceText::Paremeters textParams{
        .pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf",
        .textSize = kStandardUITextSize,
        .color = ColorPresets::white
    };

    InterfaceNode* nodes[kButtonCount]{};
    BB_FRectangle buttonRect{ 0, 0, kButtonWidth, kButtonHeight};

    // Create buttons in 2x2 grid
    for ( int index = 0 ; index < kButtonCount; ++index )
    {
        int x = gridX( index );
        int y = gridY( index );

        btnParams.callbackFunction = [this, index, actionStr = actions[index]](){OnCombatButtonPressed( actionStr );};

        std::string name = actions[index];
        name += "_button";
        buttonRect.x = kXStart + x * (kButtonWidth + kXPad);
        buttonRect.y = kYStart + y * (kButtonHeight + kYPad);
        nodes[index] = pBackground->MakeChildNode<InterfaceButton>( name.c_str(), buttonRect, btnParams);

        name += "_label";
        buttonRect.x = 0;
        buttonRect.y = 0;
        textParams.pText = actions[index];
        nodes[index]->MakeChildNode<InterfaceText>( name.c_str(), buttonRect, textParams );

        if ( index > 0 )
        {
            if ( x > 0 )
            {
                int leftNodeIndex = gridIndex( x - 1, y );
                nodes[index]->SetAdjacentNode( kLeft, nodes[leftNodeIndex] );
                nodes[leftNodeIndex]->SetAdjacentNode( kRight, nodes[index] );
            }
            if ( y > 0 )
            {
                int aboveIndex = gridIndex( x, y - 1 );
                nodes[index]->SetAdjacentNode( kUp, nodes[aboveIndex] );
                nodes[aboveIndex]->SetAdjacentNode( kDown, nodes[index] );
            }
        }
    }
    m_combatRoot.GetHighlight()->SetTarget(nodes[0]);
    m_combatRoot.SetCursorTarget( nodes[0] );
}

void EncounterComponent::OnCombatButtonPressed(const std::string& action)
{
    if (action == "Fight")
    {
        PlayerAttack();
    }
    else if (action == "Spell")
    {
        ShowActionMessage("You have no Spell yet!");
    }
    else if (action == "Item")
    {
        auto* pInventory = m_pPlayer->GetComponent<InventoryComponent>();
        if (!pInventory)
        {
            ShowActionMessage("\'Nothing of use has yet been given to thee\'");
            return;
        }

        std::vector<std::pair<std::string, int>> items;

        // Example inventory queries — adapt to your InventoryComponent
        if (pInventory->GetHasTorch())   items.push_back({ "Torch", 1 });
        if (pInventory->GetHasTablet())  items.push_back({ "Tablet", 1 });
        if (pInventory->GetHasClub())   items.push_back({ "Club", 1 });
        if (pInventory->GetHasLeatherClothes())  items.push_back({ "Leather Armor", 1 });
        if (items.empty())
        {
            ShowActionMessage("'Nothing of use has yet been given to thee'");
            return;
        }

        ShowItemMenu(items);
    }
    else if (action == "Run")
    {
        TryToFlee();
    }
}

void EncounterComponent::ShowActionMessage(const std::string& text)
{
    DismissActionMessage();

    // --- Text parameters ---
    BB_FRectangle txtRect{kTileSize, kTileSize / 2, kMessageBoxWidth - kTileSize, kMessageBoxHeight - kTileSize};
    InterfaceText::Paremeters params
    {
        .pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf",
        .pText = text.c_str(),
        .textSize = kStandardUITextSize,
        .color = ColorPresets::white,
    };

    m_pMessageBackground->MakeChildNode<InterfaceText>( "message_log_text", txtRect, params );

}

void EncounterComponent::DismissActionMessage()
{
    m_pMessageBackground->RemoveAllChildNodes();
}

void EncounterComponent::RespawnPlayer()
{
    // --- Start fade-out immediately ---
    ScreenFader::FadeToBlack(1.0f);

    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage( kLevelChanging, m_pOwner );

    // --- Delay the reset until fade is complete ---
    auto delayFunc = [pPlayer = m_pPlayer]() -> void
    {
        BlackBoxGame::Get()->GetGameStarter().StartGamePlayer( pPlayer );

        // --- Fade back in ---
        ScreenFader::FadeIn(1.0f);
        BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage( kLevelChangEnd, nullptr );
    };

    // Run reset AFTER the fade finishes (≈ 1 second)
    DelayedCallbackManager::AddCallback(delayFunc, std::chrono::milliseconds(1000));
}


bool EncounterComponent::CreateItemBox()
{
    static constexpr float kItemBoxStartX = 10 * 16;  // right side of the screen
    static constexpr float kItemBoxStartY = 3 * 16;
    static constexpr float kItemBoxWidth = 6 * 16;
    static constexpr float kItemBoxPadding = 8.f;

    m_itemMenuInterface.SetInterfaceKeys({ .select = kSelectkey });
    m_itemMenuInterface.GetRoot()->SetOffset(kItemBoxStartX, kItemBoxStartY);

    // Start with an empty box
    BB_FRectangle bgRect{ 0, 0, kItemBoxWidth, 16 };  // Height will expand later
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/BottomTextBox.png",
        .useFullImage = true
    };
    m_pItemBackgroundNode = m_itemMenuInterface.AddNode<InterfaceTexture>("ItemMenu_BG", bgRect, bgInfo);

    InterfaceButton::ButtonParams closeBtn{
        .usable = true,
        .color = {0,0,0,0},
        .targetedColor = {0,0,0,0},
        .interactColor = {0,0,0,0},
        .callbackFunction = [this]() { CloseItemMenu(); }
    };
    auto* pCloseBtn = m_itemMenuInterface.AddNode<InterfaceButton>("CloseItemButton", { 0,0,0,0 }, closeBtn);
    m_itemMenuInterface.SetCursorTarget(pCloseBtn);

    return true;
}

void EncounterComponent::ShowItemMenu(const std::vector<std::pair<std::string, int>>& items)
{
    if (m_itemMenuActive)
        return;
    m_itemMenuActive = true;

    // Clear any existing UI nodes
    m_itemMenuInterface.GetRoot()->RemoveAllChildNodes();

    static constexpr float kItemBoxWidth = 6 * 16;
    static constexpr float kLineHeight = 20.f;
    static constexpr float kTextStartX = 10.f;
    static constexpr float kTextStartY = 10.f;

    // Compute height dynamically
    float totalHeight = (items.size() * kLineHeight) + 20.f;

    // Recreate background texture with new height
    BB_FRectangle bgRect{ 0, 0, kItemBoxWidth, totalHeight };
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/BottomTextBox.png",
        .useFullImage = true
    };
    m_pItemBackgroundNode = m_itemMenuInterface.AddNode<InterfaceTexture>("ItemMenu_BG", bgRect, bgInfo);

    // Add text for each item
    InterfaceText::Paremeters textParams{
        .pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf",
        .textSize = kStandardUITextSize,
        .color = ColorPresets::white
    };

    int index = 0;
    for (auto& [name, count] : items)
    {
        std::string text = name + "   " + std::to_string(count);
        BB_FRectangle txtRect{ kTextStartX, kTextStartY + (index * kLineHeight), 100, kLineHeight };
        textParams.pText = text.c_str();
        m_pItemBackgroundNode->MakeChildNode<InterfaceText>(
            ("Item_" + name).c_str(), txtRect, textParams);
        index++;
    }

    // Add invisible close button for exiting
    InterfaceButton::ButtonParams closeBtn{
        .usable = true,
        .color = {0,0,0,0},
        .targetedColor = {0,0,0,0},
        .interactColor = {0,0,0,0},
        .callbackFunction = [this]() { CloseItemMenu(); }
    };
    auto* pCloseBtn = m_itemMenuInterface.AddNode<InterfaceButton>("CloseItemButton", { 0,0,0,0 }, closeBtn);
    m_itemMenuInterface.SetCursorTarget(pCloseBtn);

    // Add to screen
    m_itemMenuInterface.AddToScreen();
    BlackBoxManager::Get()->m_pInputManager->SwapInputTargetToInterface(&m_itemMenuInterface);
    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage(kMessageUIOpen, m_pOwner);
}

void EncounterComponent::CloseItemMenu()
{
    if (!m_itemMenuActive) return;

    m_pItemBackgroundNode->RemoveAllChildNodes();
    m_itemMenuInterface.RemoveFromScreen();
    m_itemMenuActive = false;

    // Return control to command menu
    if (m_pCommandBackground)
        BlackBoxManager::Get()->m_pInputManager->SwapInputTargetToInterface(&m_combatRoot);
    else
        BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();

    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage(kMessageUIClosed, m_pOwner);
}