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
#include "../Black Box Engine/Graphics/Text Rendering/Text.h"



static constexpr int kTileSize = 16;

static constexpr float kMessageBoxStartX = 2 * kTileSize;
static constexpr float kMessageBoxStartY = 9.5 * kTileSize;
static constexpr float kMessageBoxWidth =  12 * kTileSize;
static constexpr float kMessageBoxHeight = 5 * kTileSize;
static constexpr float kStandardUITextSize = 26.f;
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

static constexpr float kItemBoxStartX = 10 * 16;  // right side of the screen
static constexpr float kItemBoxStartY = 3 * 16;


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
    
    ScrollingTextBox::Params params{
        .scrollSpeed = 30.f,
    };
    constexpr BB_FRectangle textRect{kTileSize, kTileSize /  2, 0, 0};
    m_pTextBox = m_pMessageBackground->MakeChildNode<ScrollingTextBox>( "TextBox", textRect, params );

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
    ShowActionMessage( std::format( "A {} draws\nnear!\n\nCommand?", m_name ) );
}

void EncounterComponent::EndEncounter()
{
    BlackBoxManager::Get()->m_pAudioManager->SetMusicTrack("../Assets/Audio/05DragonQuest1-KingdomofAlefgard.wav");

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
    
    m_playerDead = true;

    ShowActionMessage( "Thou art dead." );

    BlackBoxManager::Get()->m_pInputManager->StopAllInput();
    auto delayFunc = [this]()
        { 
            EndEncounter();
            BlackBoxManager::Get()->m_pActorManager->DestroyActor( m_pOwner );
            RespawnPlayer();
        };
    DelayedCallbackManager::AddCallback( delayFunc, std::chrono::milliseconds( 4000 ) );
}

void EncounterComponent::Update()
{
    m_combatRoot.Update();
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
        {
            BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/40DragonQuest1-Missed!.wav", 0.4f);
            ShowActionMessage("The Drakee misses!\n\nCommand?");
        }
        else
            BasicAttack();
    }
    else if (m_name == "Ghost")
    {
        if (roll < 0.25f)
        {
            BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/40DragonQuest1-Missed!.wav", 0.4f);
            ShowActionMessage("The Ghost fades \naway, dodging your\nstrike!");
        }                                        
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
    BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/36DragonQuest1-Attack.wav", 0.4f);
    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pStats) 
        return;

    int roll = m_randomMachine.GetRandomInRange(64);
    if (roll < m_dodgeChance)
    {
        BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/40DragonQuest1-Missed!.wav", 0.4f);
                                                                                            
        ShowActionMessage(std::format("Thou attack!\nThe attack failed \nand there was no \nloss of Hit Points!\n\n"));
        EnemyTakeTurn();
        return;
    }

    auto* pInventory = m_pPlayer->GetComponent<InventoryComponent>();

    int playerAtk;

    if (pInventory->GetHasClub() == true)
    {
        playerAtk = pStats->GetPlayerStrength() + 4;
    }
    else
        playerAtk = pStats->GetPlayerStrength();

    int damage = std::max(1, playerAtk - m_defense);
    m_hp -= damage;

    DelayedCallbackManager::AddCallback([this]() {
    BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/37DragonQuest1-ReceiveDamage.wav", 0.4f);
        }, std::chrono::milliseconds(500));
    ShowActionMessage(
                                                       
        std::format("Thou attack!\nThe {}'s\nHit Point\nhave been reduced\nby {}.\n\n", m_name, damage),
        [this]()
        {
            BlackBoxManager::Get()->m_pInputManager->StopAllInput();
            // Only run after text finishes scrolling
            if (m_hp <= 0)
            {
                DelayedCallbackManager::AddCallback([this]() {
                    BlackBoxManager::Get()->m_pAudioManager->RemoveMusicTrack();
                    BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/25DragonQuest1-Victory.wav", 0.4f);
                    
                    if ( m_pPlayer )
                    {
                        auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
                        if ( !pStats ) return;

                        pStats->SetPlayerExperience( pStats->GetPlayerExperience() + m_xpReward );
                        pStats->SetPlayerGold( pStats->GetPlayerGold() + m_goldReward );
                    }
                    ShowActionMessage(
                        std::format("Thou hast done well\nin defeating the \n{}.\n\nThy Experience \nincreases by {}.\nThy gold increases\nby {}.",
                            m_name, m_xpReward, m_goldReward),
                        [this]() { 
                            EndEncounter(); }
                    );
                }, std::chrono::milliseconds(500));
            }
            else
            {
                    EnemyTakeTurn();
            }
        }
    );

   
}

void EncounterComponent::TryToFlee()
{
    BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/35DragonQuest1-ExcellentMove.wav", 0.4f);
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
        ShowActionMessage("Thou started to \nrun away.");
        EndEncounter();    
    }
    else
    {                      
        ShowActionMessage("Thou started to \nrun away.\nBut was blocked \n in front.\n\n");
        DelayedCallbackManager::AddCallback([this]() {
            EnemyTakeTurn();
            }, std::chrono::milliseconds(500));
    }
}


void EncounterComponent::BasicAttack()
{
    BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/36DragonQuest1-Attack.wav", 0.4f);
    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pStats) return;

    auto* pInventory = m_pPlayer->GetComponent<InventoryComponent>();
    int defense;

    if (pInventory->GetHasLeatherClothes() == true)
    {
        defense = pStats->GetPlayerStrength() + 2;
    }
    else
        defense = pStats->GetPlayerStrength();  // use real defense

    int damage = std::max(1, m_attack - defense);

    int currentHP = pStats->GetPlayerHP();
    pStats->SetPlayerHP(currentHP - damage);

    DelayedCallbackManager::AddCallback([this]() {
        BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/37DragonQuest1-ReceiveDamage.wav", 0.4f);
        }, std::chrono::milliseconds(500));


    ShowActionMessage(std::format("The {} \nattacks!\nThy Hit decreased \nby {}.\n\n", m_name, damage) );                                

    // Check if player died
    if (pStats->GetPlayerHP() <= 0)
    {
        BlackBoxManager::Get()->m_pAudioManager->RemoveMusicTrack();
        BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/20DragonQuest1-ThouHastDied.wav", 0.4f);
        PlayerDies();
    }
    else
        ShowActionMessage(std::format("Command?\n\n"));
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

        BlackBoxManager::Get()->m_pAudioManager->PlaySound( "../Assets/Audio/38DragonQuest1-ReceiveDamage(2).wav", 0.4f );
        ShowActionMessage(std::format("The {}\n casts Hurt!\nYou take {} damage!", m_name, damage));

        if (pStats->GetPlayerHP() <= 0)
        {
            BlackBoxManager::Get()->m_pAudioManager->RemoveMusicTrack();
            BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/20DragonQuest1-ThouHastDied.wav", 0.4f);
            PlayerDies();
        }
        else
            ShowActionMessage(std::format("Command?\n\n"));
    }
}

void EncounterComponent::StartCombatUI()
{
    BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/32DragonQuest1-MenuButton.wav", 0.4f);
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
    if ( m_pTextBox && m_pTextBox->IsAnimating())
        return;

    BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/32DragonQuest1-MenuButton.wav", 0.4f);

    if (action == "Fight")
    {
        PlayerAttack();
    }
    else if (action == "Spell")
    {
        ShowActionMessage("Thou cannot yet \nuse the spell.\n\nCommand?");
    }                      
    else if (action == "Item")
    {
        auto* pInventory = m_pPlayer->GetComponent<InventoryComponent>();
        if (!pInventory)
        {
            ShowActionMessage("\'Nothing of use has\nyet been given to\nthee\'");
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
            ShowActionMessage("'Nothing of use has\nyet been given to \nthee'");
            return;
        }

        ShowItemMenu(items);
    }
    else if (action == "Run")
    {
        TryToFlee();
    }
}

void EncounterComponent::ShowActionMessage(const std::string& text , std::function<void()> onComplete )
{
   m_pTextBox->SetText(text , onComplete );
}

void EncounterComponent::DismissActionMessage()
{
    m_pMessageBackground->RemoveAllChildNodes();
    m_pTextBox = nullptr;
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
    static constexpr float kItemBoxWidth = 120.f;     // wider to fit text

    // --- Setup interface keys ---
    m_itemMenuInterface.SetInterfaceKeys({ .select = kSelectkey });
    m_itemMenuInterface.GetRoot()->SetOffset(kItemBoxStartX, kItemBoxStartY);

    // --- Background box (persistent) ---
    BB_FRectangle bgRect{ 0, 0, kItemBoxWidth, 16 };  // temporary height, will expand
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/BottomTextBox.png",
        .useFullImage = true
    };
    m_pItemBackgroundNode = m_itemMenuInterface.AddNode<InterfaceTexture>("ItemMenu_BG", bgRect, bgInfo);

    // --- Setup highlight like combat menu ---
    auto* highlight = m_itemMenuInterface.GetHighlight();
    highlight->SetParameters({
        .mode = InterfaceHighlighter::kModeIcon,
        .pSpriteFile = "../Assets/UI/Icons/IconSpriteFile.xml",
        .iconOffset{-5, 0},  // same arrow offset
        .iconSize{4, 7}
        });

    return true;
}

void EncounterComponent::ShowItemMenu(const std::vector<std::pair<std::string, int>>& items)
{
    if (m_itemMenuActive) return;
    m_itemMenuActive = true;

    // Clear previous nodes and storage
    m_pItemBackgroundNode->RemoveAllChildNodes();
    m_itemTextStorage.clear(); // member: std::vector<std::string> m_itemTextStorage;

    static constexpr float kLineHeight = 20.f;
    static constexpr float kTextStartY = 10.f;
    static constexpr float kItemBoxWidth = 96.f;

    // Resize background to fit items
    float totalHeight = items.size() * kLineHeight + 2 * kTextStartY;
    m_pItemBackgroundNode->SetSize(kItemBoxWidth, totalHeight);

    InterfaceText::Paremeters textParams{
        .pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf",
        .textSize = kStandardUITextSize,
        .color = ColorPresets::white
    };

    InterfaceButton::ButtonParams btnParams{
        .usable = true,
        .color = {0,0,0,0},
        .targetedColor = {0,0,0,0},
        .interactColor = {0,0,0,0},
    };

    std::vector<InterfaceNode*> itemButtons;
    int index = 0;

    for (auto& [name, count] : items)
    {
        BB_FRectangle buttonRect{ 0, kTextStartY + index * kLineHeight, kItemBoxWidth, kLineHeight };
        std::string btnName = "ItemBtn_" + name;

        // Store the string so c_str() pointer is valid
        std::string itemLabelText = name + "  " + std::to_string(count);
        m_itemTextStorage.push_back(itemLabelText);

        btnParams.callbackFunction = [this, name]()
            {
                auto* pInventory = m_pPlayer->GetComponent<InventoryComponent>();
                if (!pInventory) return;

                if ((name == "Torch" && pInventory->GetHasTorch()) ||
                    (name == "Tablet" && pInventory->GetHasTablet()) ||
                    (name == "Club" && pInventory->GetHasClub()) ||
                    (name == "Leather Armor" && pInventory->GetHasLeatherClothes()))
                {                                  
                    ShowActionMessage(std::format("Thou canst not use\nit in battle."));
                }

                CloseItemMenu(); // Return to combat menu
            };

        InterfaceNode* btnNode = m_pItemBackgroundNode->MakeChildNode<InterfaceButton>(btnName.c_str(), buttonRect, btnParams);

        // Text relative to button
        BB_FRectangle txtRect{ 16, 0, buttonRect.w - 20, buttonRect.h };
        textParams.pText = m_itemTextStorage.back().c_str(); // safe pointer
        btnNode->MakeChildNode<InterfaceText>((btnName + "_Label").c_str(), txtRect, textParams);

        itemButtons.push_back(btnNode);
        index++;
    }

    // Link buttons for up/down navigation
    for (int i = 0; i < itemButtons.size(); ++i)
    {
        if (i > 0) itemButtons[i]->SetAdjacentNode(Direction::kUp, itemButtons[i - 1]);
        if (i < itemButtons.size() - 1) itemButtons[i]->SetAdjacentNode(Direction::kDown, itemButtons[i + 1]);
    }

    // Highlight first item with arrow
    if (!itemButtons.empty())
    {
        m_itemMenuInterface.SetCursorTarget(itemButtons[0]);
        auto* highlighter = m_itemMenuInterface.GetHighlight();

        highlighter->SetParameters({
            .mode = InterfaceHighlighter::kModeIcon,
            .pSpriteFile = "../Assets/UI/Icons/IconSpriteFile.xml",
            .iconOffset = {5, 0},
            .iconSize = {4, 7}
            });

        highlighter->SetTarget(itemButtons[0]);
    }

    // Add menu to screen
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



