#include "InteractionComponent.h"

#include <Actors/ActorManager.h>
#include <BlackBoxManager.h>
#include <System/Delay.h>

#include "BlackBoxGame.h"
#include "PlayerMovementComponent.h"
#include "StairComponent.h"
#include "TileSystem/EncounterComponent.h"
#include "PlayerStatsComponent.h"
#include "GameMessages.h"

using namespace BlackBoxEngine;

InteractionComponent::InteractionComponent( BlackBoxEngine::Actor* pOwner )
    : Component(pOwner)
{
    bool good = CreateCommandMenuUI();
    if ( !good )
    {
        BB_LOG( LogType::kError, "Failed to create command menu ui" );
    }

}

InteractionComponent::~InteractionComponent()
{
    auto* pInput = BlackBoxManager::Get()->m_pInputManager;
    using enum InputManager::InputType;
    for ( auto id : m_keyDownCodes )
        pInput->UnsubscribeKey( id, kKeyDown );
}

// -------------------------------------------------------------
// Start
// -------------------------------------------------------------
void InteractionComponent::Start()
{
    auto* input = BlackBoxManager::Get()->m_pInputManager;
    using enum InputManager::InputType;

    // X = open UI
    m_keyDownCodes[0] = 
        input->SubscribeToKey( KeyCode::kX, kKeyDown, [this]() { if ( !m_uiActive ) OpenUI(); } );

    // Z = close UI
    m_keyDownCodes[1] =
        input->SubscribeToKey( KeyCode::kZ, kKeyDown, [this]() { if ( m_uiActive ) CloseUI(); } );
}

// -------------------------------------------------------------
// Update
// -------------------------------------------------------------
void InteractionComponent::Update()
{
}

// -------------------------------------------------------------
// OnCollide
// -------------------------------------------------------------
void InteractionComponent::OnCollide(Actor* other)
{
    if (!other) return;

    if (auto* stair = other->GetComponent<BaseStairComponent>())
    {
        m_pCurrentStair = stair;
        m_pCurrentTalk = nullptr;
        m_pCurrentTake = nullptr;
        return;
    }

    if (auto* talk = other->GetComponent<TalkComponent>())
    {
        m_pCurrentTalk = talk;
        m_pCurrentStair = nullptr;
        m_pCurrentTake = nullptr;
        return;
    }

    if (auto* take = other->GetComponent<TakeComponent>())
    {
        m_pCurrentTake = take;
        m_pCurrentStair = nullptr;
        m_pCurrentTalk = nullptr;
        return;
    }

    // None found
    m_pCurrentStair = nullptr;
    m_pCurrentTalk = nullptr;
    m_pCurrentTake = nullptr;
}

// -------------------------------------------------------------
// UI: Open/Close
// -------------------------------------------------------------
void InteractionComponent::OpenUI()
{
    if ( m_uiActive )
        return;
    m_uiActive = true;

    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage( kMessageUIOpen, m_pOwner);
    BlackBoxManager::Get()->m_pAudioManager->PlaySound( "../Assets/Audio/32DragonQuest1-MenuButton.wav"  , 0.2f );

    m_pCommandMenuRootNode.AddToScreen();

    auto* input = BlackBoxManager::Get()->m_pInputManager;
    input->SwapInputTargetToInterface( &m_pCommandMenuRootNode );
}

void InteractionComponent::CloseUI()
{
    m_pCommandMenuRootNode.RemoveFromScreen();
    m_messageRoot.RemoveFromScreen();

    BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();

    m_uiActive = false;
    m_messageActive = false;
    m_messageNode = nullptr;

    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage( kMessageUIClosed, m_pOwner );
}


// -------------------------------------------------------------
// UI: Selection Menu
// -------------------------------------------------------------
bool InteractionComponent::CreateCommandMenuUI()
{
    using enum Direction;

    static constexpr float kButtonWidth = 42.f;
    static constexpr float kButtonHeight = 7.f;
    static constexpr float kYPad = 3.f;
    static constexpr float kXPad = 3.f;
    static constexpr int kButtonCount = 4;
    static constexpr int kButtonGridWidth = 2;
    static constexpr int kButtonGridHeight = kButtonCount / kButtonGridWidth;

    auto gridX = []( int i ) { return i % kButtonGridWidth; };
    auto gridY = []( int i ) { return i / kButtonGridWidth; };
    auto gridIndex = []( int x, int y ) { return x + (y * kButtonGridHeight); };

    // Offset menu
    m_pCommandMenuRootNode.GetRoot()->SetOffset( 88, 16 );

    // Highlighter
    auto* highlighter = m_pCommandMenuRootNode.GetHighlight();
    if ( !highlighter )
    {
        BB_LOG( LogType::kError, "UI highlighter not found" );
        return false;
    }

    highlighter->SetParameters( {
        .mode = InterfaceHighlighter::kModeIcon,
        .pSpriteFile = "../Assets/UI/Icons/IconSpriteFile.xml",
        .iconOffset{-5, -2},
        .iconSize{4, 7}
        } );

    // Background box
    static constexpr int kBackgroundWidth = 8 * 16;
    static constexpr int kBackgroundHeight = 6 * 16;
    BB_FRectangle bgRect{0, 0 , kBackgroundWidth, kBackgroundHeight};
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/SelectionBox.png",
        .spriteDimensions = {kBackgroundWidth, kBackgroundHeight},
        .useFullImage = true
    };
    auto* pBackground = m_pCommandMenuRootNode.AddNode<InterfaceTexture>( "UI_Background", bgRect, bgInfo );

    // Button setup
    InterfaceButton::ButtonParams btnParams{
        .usable = true,
        .color = ColorValue( 0, 0, 0, 0 ),
        .targetedColor = ColorValue( 0, 0, 0, 0 ),
        .interactColor = ColorValue( 0, 0, 0, 0 )
    };
    // Text labels
    InterfaceText::Paremeters textParams{
        .pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf",
        .textSize = 16,
        .color = ColorPresets::white
    };

    static constexpr const char* labels[kButtonCount] = {"Talk", "Stair", "Take", "Item"};
    static constexpr const char* actions[kButtonCount] = {"talk", "stair", "take", "item"};
    InterfaceNode* nodes[kButtonCount] = {};

    BB_FRectangle rect{0, 0, kButtonWidth, kButtonHeight};

    for ( int i = 0; i < kButtonCount; ++i )
    {
        int x = gridX( i );
        int y = gridY( i );

        std::string name = actions[i];
        name += +"_button";

        rect.x = kXPad + x * (kXPad + kButtonWidth);
        rect.y = kYPad + y * (kYPad + kButtonHeight);
        btnParams.callbackFunction = [this, i, actionStr = actions[i]]() { OnButtonPressed( actionStr ); };
        nodes[i] = pBackground->MakeChildNode<InterfaceButton>( name.c_str(), rect, btnParams );

        rect.x = 0;
        rect.y = 0;
        name += "_label";
        textParams.pText = labels[i];
        nodes[i]->MakeChildNode<InterfaceText>(name.c_str(), rect, textParams);

        if ( i > 0 )
        {
            if ( x > 0 )
            {
                int leftNodeIndex = gridIndex( x - 1, y );
                nodes[i]->SetAdjacentNode( kLeft, nodes[leftNodeIndex] );
                nodes[leftNodeIndex]->SetAdjacentNode( kRight, nodes[i] );
            }
            if ( y > 0 )
            {
                int aboveIndex = gridIndex( x, y - 1 );
                nodes[i]->SetAdjacentNode( kUp, nodes[aboveIndex] );
                nodes[aboveIndex]->SetAdjacentNode( kDown, nodes[i] );
            }
        }
    }

    m_pCommandMenuRootNode.SetInterfaceKeys( kCommandKeys );

    highlighter->SetTarget( nodes[0] );
    m_pCommandMenuRootNode.SetCursorTarget( nodes[0] );

    auto* inputTarget = m_pCommandMenuRootNode.GetInputTarget();
    inputTarget->m_keyDown.RegisterListener( KeyCode::kZ, [this]() { CloseUI(); } );

    return true;
}

// -------------------------------------------------------------
// Message Box
// -------------------------------------------------------------
void InteractionComponent::ShowActionMessage(const std::string& text)
{
    if (m_messageActive) return;

    m_messageActive = true;

    constexpr BB_FRectangle bgRect{ 60, 60, 104, 24 };
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/BottomTextBox.png",
        .spriteDimensions = {16, 16},
        .useFullImage = true
    };
    m_messageRoot.AddNode<InterfaceTexture>("ActionMessage_BG", bgRect, bgInfo);

    BB_FRectangle txtRect{ 72.f, 70.f, 100.f, 20.f };
    InterfaceText::Paremeters params; // create default struct

    params.pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf";
    params.textSize = 16;
    params.color = ColorPresets::white;
    params.pText = text.c_str();

    m_messageNode = m_messageRoot.AddNode<InterfaceText>("ActionMessage_Text", txtRect, params);
    m_messageRoot.AddToScreen();
}

void InteractionComponent::DismissActionMessage()
{
    if (!m_messageActive) return;
    m_messageRoot.RemoveFromScreen();
    m_messageNode = nullptr;
    m_messageActive = false;
}

// -------------------------------------------------------------
// Action Handling
// -------------------------------------------------------------
void InteractionComponent::OnButtonPressed(const std::string& action)
{
    static const std::unordered_map<std::string, void(InteractionComponent::*)()> kActionMap = {
        {"talk", &InteractionComponent::HandleTalk},
        {"stair", &InteractionComponent::HandleStair},
        {"take", &InteractionComponent::HandleTake},
    };

    auto it = kActionMap.find(action);
    if (it != kActionMap.end())
        (this->*(it->second))();
    else
        ShowActionMessage("Unknown action.");
}

void InteractionComponent::HandleTalk()
{
    if (m_pCurrentTalk)
        ShowActionMessage("Trading with Inn.");
    else
        ShowActionMessage("There is no one there.");
}

void InteractionComponent::HandleStair()
{
    if (m_pCurrentStair)
    {
        CloseUI();
        m_didMove = true;
        m_pCurrentStair->OnStairUsed(m_playerActor);
    }
    else
        ShowActionMessage("Thou canst not go down.");
}

void InteractionComponent::HandleTake()
{
    if (m_pCurrentTake)
    {
        //CloseUI();
        //m_didMove = true;
        ShowActionMessage("You found a key.");
    }
    else
        ShowActionMessage("There is nothing to take here.");
}

// -------------------------------------------------------------
// Level Transition
// -------------------------------------------------------------
void InteractionComponent::OnLevelTransitionStart()
{
    auto* player = m_pOwner->GetComponent<PlayerStatsComponent>();
    if (!player) return;

    m_isChangingLevel = true;
    if (player->m_hudVisible) 
        player->HideHUD();

    if (m_delayedDisplayId != 0)
    {
        DelayedCallbackManager::RemoveCallback(m_delayedDisplayId);
        m_delayedDisplayId = 0;
    }
}

void InteractionComponent::OnLevelTransitionEnd()
{
    m_isChangingLevel = false;
}

void InteractionComponent::Render() {}
void InteractionComponent::Save(BlackBoxEngine::XMLElementParser) {}
void InteractionComponent::Load(const BlackBoxEngine::XMLElementParser) {}