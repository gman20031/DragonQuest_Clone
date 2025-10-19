#include "EncounterComponent.h"
#include <Resources/ResourceManager.h>

#include <Actors/ActorManager.h>
#include <BlackBoxManager.h>
#include <Actors/EngineComponents/SpriteComponent.h>
#include <Graphics/ScreenFader.h>
#include <Actors/EngineComponents/TransformComponent.h>
#include <System/Delay.h>
#include <format>

#include "../Interactions/InteractionComponent.h"
#include "../PlayerMovementComponent.h"
#include "../GameMessages.h"
#include "EncounterHandler.h"

static constexpr float kMessageBoxWidth = 12 * 16;
static constexpr float kMessageBoxHeight = 5 * 16;
static constexpr float kStandardUITextSize = 28;
static constexpr KeyCode kSelectkey = KeyCode::kX;
static constexpr KeyCode kBackKey = KeyCode::kZ;

using namespace BlackBoxEngine;

EncounterComponent::EncounterComponent( BlackBoxEngine::Actor* pOwner )
    : Component( pOwner )
{
    m_randomMachine.Reset( std::time( 0 ) );

    static constexpr float kMessageBoxStartX = 2 * 16;
    static constexpr float kMessageBoxStartY = 9.5 * 16;

    constexpr BB_FRectangle bgRect{kMessageBoxStartX, kMessageBoxStartY, kMessageBoxWidth, kMessageBoxHeight};
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/BottomTextBox.png",
        .useFullImage = true
    };
    m_pMessageBackground = m_combatRoot.AddNode<InterfaceTexture>( "ActionMessage_BG", bgRect, bgInfo );

    m_combatRoot.SetInterfaceKeys( UserInterface::InterfaceKeys{
            .select = kSelectkey
    } );
}

void EncounterComponent::StartEncounter(Actor* pOtherActor)
{
    m_inBattle = true;
    BB_LOG(LogType::kMessage, "Enemy '%s' appeared! HP=%d", m_name.c_str(), m_hp);
    
    if ( !pOtherActor->GetComponent<PlayerStatsComponent>() )
    {
        BB_LOG( LogType::kError, "Actor passed into startEncounter, did not have player stat component" );
        return;
    }

    BlackBoxManager::Get()->m_pAudioManager->SetMusicTrack( "../Assets/Audio/24DragonQuest1-MonsterBattle.wav" );

    m_pPlayer = pOtherActor;
    StartCombatUI();
}

void EncounterComponent::EndEncounter()
{

    if (auto* playerMove = m_pPlayer->GetComponent<PlayerMovementComponent>())
    {
        playerMove->SetAnimationPaused(false);
        playerMove->m_stopMoving = false;
    }
    BlackBoxManager::Get()->m_pAudioManager->SetMusicTrack( "../Assets/Audio/05DragonQuest1-KingdomofAlefgard.wav" );

    m_inBattle = false;
    BB_LOG(LogType::kMessage, "Encounter ended.");
}

void EncounterComponent::Update()
{
    if (m_waitingForExit)
    {
        auto* input = BlackBoxManager::Get()->m_pInputManager;

        if (input->IsKeyDown(KeyCode::kX)) // adjust to your engine’s input API
        {
            m_waitingForExit = false;

            DismissActionMessage();
            EndEncounter(); 
            BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();
        }
    }
}

void EncounterComponent::EnemyTakeTurn()
{
    //check if the enemy won
    if (m_hp <= 0)
    {
        m_battleState = BattleState::Victory;
        return;
    }

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
        {
            ShowActionMessage("The Drakee misses!");
        }
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
    else if (m_name == "Spellian")
    {
        if (roll < 0.5f)
            CastSpell("Hurt");
        else
            BasicAttack();
    }
    else
        BasicAttack();
}


void EncounterComponent::PlayerAttack()
{
    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pStats) return;

 
    if (m_name == "Ghost")
    {
        int roll = m_randomMachine.GetRandomInRange(64);
        if (roll < 4)
        {
            ShowActionMessage(std::format("The {} dodges your attack!", m_name.c_str()));
            auto delayFunc = [this]() -> void {
                EnemyTakeTurn();
                };
            DelayedCallbackManager::AddCallback(delayFunc, std::chrono::milliseconds(1000));
            return;
            return;
        }
    }

    int playerAtk = pStats->GetPlayerStrength();
    int damage = std::max(1, playerAtk - m_defense);
    m_hp -= damage;

    ShowActionMessage(std::format("You hit the {} for {} damage!", m_name, damage));

    pStats->RefreshHUD();

    if (m_hp <= 0)
    {
        //pStats->Set(m_xpReward); //i need the XP
        pStats->SetPlayerGold(pStats->GetPlayerGold() + m_goldReward);
        
        pStats->RefreshHUD();

        EndCombatUI();
        ShowActionMessage(std::format("The {} is defeated!", m_name.c_str()));
        m_waitingForExit = true;
        
    }
    else
    {
        auto delayFunc = [this]() -> void {
            EnemyTakeTurn();
            };
        DelayedCallbackManager::AddCallback(delayFunc, std::chrono::milliseconds(1000));
        return;
    }

}

void EncounterComponent::TryToFlee()
{
    if (!m_inBattle)
        return;

    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pStats) return;

    // --- Calculate dynamic flee chance based on agility difference ---
    // If player Agility is higher than the monster’s, easier to flee; lower makes it harder.
    // The formula yields roughly:
    //   - 0.9f max chance (easy escape)
    //   - 0.1f min chance (very difficult)
    float agilityDiff = static_cast<float>(pStats->GetPlayerAgility() - m_agility);
    float fleeChance = std::clamp(0.5f + agilityDiff * 0.02f, 0.1f, 0.9f);

    // --- Optional per-enemy tweak (if you want unique behavior) ---
    if (m_name == "Ghost")
        fleeChance *= 0.8f; // Slightly harder to escape Ghosts (eerie persistence)
    else if (m_name == "Spellian")
        fleeChance *= 0.9f; // Spellians may try to trap you with Spell

    // --- Roll the outcome ---
    if ( m_randomMachine.GetRandomInRange( 1.0f ) < fleeChance)
    {
        EndCombatUI();
        ShowActionMessage("You successfully escaped!");
        m_waitingForExit = true;
        m_inBattle = false;
    }
    else
    {
        ShowActionMessage("You cannot escape!");
        auto delayFunc = [this]() -> void {
            EnemyTakeTurn();
            };
        DelayedCallbackManager::AddCallback(delayFunc, std::chrono::milliseconds(800));
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

    pStats->RefreshHUD();

    ShowActionMessage(std::format("The {} attacks! You take {} damage!", m_name.c_str(), damage));

    // Check if player died
    if (pStats->GetPlayerHP() <= 0)
    {
        
        EndCombatUI();
        ShowActionMessage("You are defeated!");
        m_waitingForExit = true;

        auto delayFunc = [this]() -> void
            {
                RespawnPlayer();
            };

        DelayedCallbackManager::AddCallback(delayFunc, std::chrono::milliseconds(1500));
      
    }
}

void EncounterComponent::CastSpell([[maybe_unused]]const std::string& spellName)
{
    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pStats) return;

    if (spellName == "Hurt")
    {
        int damage = m_randomMachine.GetRandomInRange(8,13); // 8–13 damage
        int currentHP = pStats->GetPlayerHP();
        pStats->SetPlayerHP(std::max(0, currentHP - damage));

        pStats->RefreshHUD();

        ShowActionMessage(std::format("The {} casts Hurt! You take {} damage!", m_name.c_str(), damage));

        if (pStats->GetPlayerHP() <= 0)
        {
            EndCombatUI();
            ShowActionMessage("You are defeated!");
            m_waitingForExit = true;
        }
    }
}

//to change depending on the enemy
void EncounterComponent::Load(const XMLElementParser parser)
{
    parser.GetChildVariable("Name", &m_name);
    parser.GetChildVariable("HP", &m_hp);
    parser.GetChildVariable("Attack", &m_attack);
    parser.GetChildVariable("Defense", &m_defense);
    parser.GetChildVariable("XPReward", &m_xpReward);
    parser.GetChildVariable("GoldReward", &m_goldReward);
    parser.GetChildVariable("Agility", &m_agility);
    parser.GetChildVariable("SpriteFile", &m_spriteFile);
}

void EncounterComponent::Save(XMLElementParser parser)
{
    parser.NewChildVariable("Name", m_name);
    parser.NewChildVariable("HP", m_hp);
    parser.NewChildVariable("Attack", m_attack);
    parser.NewChildVariable("Defense", m_defense);
    parser.NewChildVariable("XPReward", m_xpReward);
    parser.NewChildVariable("GoldReward", m_goldReward);
    parser.NewChildVariable("Agility", m_agility);
    parser.NewChildVariable("SpriteFile", m_spriteFile);
}

void EncounterComponent::StartCombatUI()
{
    //m_isCombatActive = true; //DONT FORGET TO SET IT TO FALSE WHEN RUN AWAY OR DIE OR WIN
    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage( kMessageUIOpen, m_pOwner );
    static constexpr float kEnemyWidth  = 7 * 16;
    static constexpr float kEnemyHeight = 5 * 16;
    static constexpr int kCommandWindowStartX = 96;
    static constexpr int kCommandWindowStartY = 8;
    static constexpr int kCommandWindowWidth = 8 * 16;
    static constexpr int kCommandWindowHeight = static_cast<int>(3.5 * 16);

    // Command Panel
    BB_FRectangle panelRect{
        kCommandWindowStartX, kCommandWindowStartY, kCommandWindowWidth, kCommandWindowHeight
    };
    InterfaceTexture::TextureInfo panelInfo{};
    panelInfo.pTextureFile = "../Assets/UI/SelectionBox.png";
    panelInfo.useFullImage = true;
    auto* pCommandBackground = m_combatRoot.AddNode<InterfaceTexture>(
        "CommandPanelBG", panelRect, panelInfo );

    // --- Enemy sprite ---
    BB_FRectangle bgRect{-16, kCommandWindowHeight, kEnemyWidth, kEnemyHeight};
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = m_spriteFile.c_str(),
        .useFullImage = true,
    };
    pCommandBackground->MakeChildNode<InterfaceTexture>( "Combat_BG", bgRect, bgInfo );

    // highlighter
    auto* highlighter = m_combatRoot.GetHighlight();
    highlighter->SetParameters( {
        .mode = InterfaceHighlighter::kModeIcon,
        .pSpriteFile = "../Assets/UI/Icons/IconSpriteFile.xml",
        .iconOffset{-5, 0},
        .iconSize{4, 7}
        } );
    CreateCommandButtons( pCommandBackground );

    // --- Add to screen ---
    m_combatRoot.AddToScreen();
    BlackBoxManager::Get()->m_pInputManager->SwapInputTargetToInterface(&m_combatRoot);
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

void EncounterComponent::EndCombatUI()
{
    m_combatRoot.RemoveFromScreen();
    m_pPlayer->GetComponent<InteractionComponent>()->m_commandMenuActive = false;   
}

void EncounterComponent::OnCombatButtonPressed(const std::string& action)
{
    //DismissActionMessage();

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
        ShowActionMessage("You rummage through your bag...");
    }
    else if (action == "Run")
    {
        TryToFlee();
    }
}


void EncounterComponent::ShowActionMessage(const std::string& text)
{
    if ( m_messageActive )
        DismissActionMessage();

    m_messageActive = true;

    // --- Text parameters ---
    BB_FRectangle txtRect{16, 8, kMessageBoxWidth - 16, kMessageBoxHeight - 16};
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
    if (!m_messageActive) 
        return;

    m_pMessageBackground->RemoveAllChildNodes();
}

void EncounterComponent::RespawnPlayer()
{

    auto* pManager = BlackBoxManager::Get();
    if (!pManager) return;

    auto* pInteract = m_pPlayer->GetComponent<InteractionComponent>();
    if (!pInteract) return;

    auto* pTransform = m_pPlayer->GetComponent<TransformComponent>();
    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pTransform || !pStats) return;

    // --- Start fade-out immediately ---
    ScreenFader::FadeToBlack(1.0f);

    pInteract->OnLevelTransitionStart();
    pStats->HideHUD();

    // --- Delay the reset until fade is complete ---
    auto delayFunc = [pManager, pTransform, pStats, pInteract]() -> void
        {
            // --- Reset stats ---
            pStats->SetPlayerLevel(1);
            pStats->SetPlayerHP(16);
            pStats->SetPlayerMP(0);
            pStats->SetPlayerGold(120);
            pStats->SetPlayerEnergy(10);
            pStats->SetPlayerStrength(3);
            pStats->SetPlayerAgility(3);

            // --- Reset player position ---
            pTransform->m_position = { 832, 816 };
            pTransform->m_prevPosition = { 832, 816 };
            // --- Fade back in ---
            ScreenFader::FadeIn(1.0f);
            pInteract->OnLevelTransitionEnd();
        };

    // Run reset AFTER the fade finishes (≈ 1 second)
    DelayedCallbackManager::AddCallback(delayFunc, std::chrono::milliseconds(1000));
}