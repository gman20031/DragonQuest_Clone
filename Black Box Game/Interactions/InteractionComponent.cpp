#include "InteractionComponent.h"

#include <Actors/ActorManager.h>
#include <BlackBoxManager.h>
#include <System/Delay.h>

#include <Interface/InterfaceButton.h>
#include <Interface/InterfaceText.h>

#include "../Encounters/EncounterComponent.h"
#include "../PlayerMovementComponent.h"
#include "../PlayerStatsComponent.h"
#include "../BlackBoxGame.h"
#include "../GameMessages.h"
#include "StairComponent.h"
#include "../Black Box Game/InventoryComponent.h"

using namespace BlackBoxEngine;

static constexpr float kMessageBoxWidth = 12 * 16;
static constexpr float kMessageBoxHeight = 5 * 16;
static constexpr float kMessageBoxStartX = 2 * 16;
static constexpr float kMessageBoxStartY = 10 * 16;

static constexpr float kCommandBoxStartX = 7 * 16;
static constexpr float kCommandBoxStartY = 1 * 16;
static constexpr int   kCommandBoxWidth  = 8 * 16;
static constexpr int   kCommandBoxHeight = 6 * 16;

static constexpr float kStandardUITextSize = 28;

static constexpr KeyCode kSelectkey = KeyCode::kX;
static constexpr KeyCode kBackKey = KeyCode::kZ;

InteractionComponent::InteractionComponent( BlackBoxEngine::Actor* pOwner )
    : Component(pOwner)
{
    bool good = CreateCommandMenuUI();
    if ( !good )
    {
        BB_LOG( LogType::kError, "Failed to create command menu ui" );
        return;
    }
    good = CreateMessageLogBox();
    if ( !good )
    {
        BB_LOG( LogType::kError, "Failed to create message log menu ui" );
        return;
    }
}

InteractionComponent::~InteractionComponent()
{
    auto* pInput = BlackBoxManager::Get()->m_pInputManager;
    auto* pMessager = BlackBoxManager::Get()->m_pMessagingManager;
    using enum InputManager::InputType;
    for ( auto id : m_keyDownCodes )
        pInput->UnsubscribeKey( id, kKeyDown );
    for ( auto id : m_messageIds )
        pMessager->RemoveListener( id );


    
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
        input->SubscribeToKey( KeyCode::kX, kKeyDown, [this]() { if ( !m_commandMenuActive ) OpenCommandUI(); } );

    // Z = close UI
    m_keyDownCodes[1] =
        input->SubscribeToKey( KeyCode::kZ, kKeyDown, [this]() { if ( m_commandMenuActive ) CloseCommandUI(); } );

    m_messageIds.emplace_back(
        BlackBoxManager::Get()->m_pMessagingManager->RegisterListener
        ( 
            kMessageUIOpen, [this]( [[maybe_unused]] Message& )  {
                m_pCommandMenuRootNode.GetHighlight()->GetSprite()->StopAnimating(); 
            } 
    ));
    m_messageIds.emplace_back(
    BlackBoxManager::Get()->m_pMessagingManager->RegisterListener
    (
        kMessageUIClosed, [this]( [[maybe_unused]] Message& )
        {
            m_pCommandMenuRootNode.GetHighlight()->GetSprite()->AnimateSprite();
        }
    ));
    m_messageIds.emplace_back(
    BlackBoxManager::Get()->m_pMessagingManager->RegisterListener( 
        kLevelChanging, [this]( [[maybe_unused]]Message& ) {OnLevelChanging();} 
    ));
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

    if (auto* talk = other->GetComponent<BaseTalkComponent>())
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
// Creating UI 
// -------------------------------------------------------------
bool InteractionComponent::CreateCommandMenuUI()
{
    using enum Direction;

    static constexpr float kButtonWidth = 50.f;
    static constexpr float kButtonHeight = 14.f;
    static constexpr float kXStart = 16.f;
    static constexpr float kYStart = 16.f;
    static constexpr float kYPad = 16.f;
    static constexpr float kXPad = 0.f;
    static constexpr int kButtonCount = 4;
    static constexpr int kButtonGridWidth = 2;
    static constexpr int kButtonGridHeight = kButtonCount / kButtonGridWidth;

    auto gridX = []( int i ) { return i % kButtonGridWidth; };
    auto gridY = []( int i ) { return i / kButtonGridWidth; };
    auto gridIndex = []( int x, int y ) { return x + (y * kButtonGridHeight); };

    // Offset menu
    m_pCommandMenuRootNode.GetRoot()->SetOffset( kCommandBoxStartX, kCommandBoxStartY );

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

    BB_FRectangle bgRect{0, 0 , kCommandBoxWidth, kCommandBoxHeight};
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/SelectionBox.png",
        .spriteDimensions = {kCommandBoxWidth, kCommandBoxHeight},
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
        .textSize = kStandardUITextSize,
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

        rect.x = kXStart + x * (kXPad + kButtonWidth);
        rect.y = kYStart + y * (kYPad + kButtonHeight);
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

    m_pCommandMenuRootNode.SetInterfaceKeys( UserInterface::InterfaceKeys{
            .select = kSelectkey
    } );

    highlighter->SetTarget( nodes[0] );
    m_pCommandMenuRootNode.SetCursorTarget( nodes[0] );

    auto* inputTarget = m_pCommandMenuRootNode.GetInputTarget();
    inputTarget->m_keyDown.RegisterListener( kBackKey, [this]() { CloseCommandUI(); } );

    return true;
}

bool InteractionComponent::CreateMessageLogBox()
{
    static constexpr BB_FRectangle bgRect{0, 0, kMessageBoxWidth, kMessageBoxHeight};
    static constexpr InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/BottomTextBox.png",
        .useFullImage = true
    };
    m_messageRootInterface.SetInterfaceKeys( UserInterface::InterfaceKeys{
        .select = kSelectkey
     } );
    
    m_messageRootInterface.GetRoot()->SetOffset( kMessageBoxStartX, kMessageBoxStartY );

    m_pMessageBackgroundNode = m_messageRootInterface.AddNode<InterfaceTexture>( "ActionMessage_BG", bgRect, bgInfo );

    InterfaceButton::ButtonParams button{
        .usable = true,
        .color = {0,0,0,0},
        .targetedColor = {0,0,0,0},
        .interactColor = {0,0,0,0},
        .callbackFunction = [this]() {  DismissActionMessage(); }
    };
    auto* pButton = m_messageRootInterface.AddNode<InterfaceButton>( "RemoveMessageButton", {0,0,0,0}, button );

    m_messageRootInterface.SetCursorTarget( pButton );

    return true;
}

// -------------------------------------------------------------
// UI: Open/Close
// -------------------------------------------------------------
void InteractionComponent::OpenCommandUI()
{
    if ( m_commandMenuActive )
        return;
    m_commandMenuActive = true;

    BlackBoxManager::Get()->m_pAudioManager->PlaySound( "../Assets/Audio/32DragonQuest1-MenuButton.wav", 0.2f );

    m_pCommandMenuRootNode.AddToScreen();

    auto* input = BlackBoxManager::Get()->m_pInputManager;
    input->SwapInputTargetToInterface( &m_pCommandMenuRootNode );
    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage( kMessageUIOpen, m_pOwner );
}

void InteractionComponent::CloseCommandUI()
{
    m_pCurrentTalk = nullptr;
    m_pCurrentTake = nullptr;

    m_pCommandMenuRootNode.RemoveFromScreen();
    m_messageRootInterface.RemoveFromScreen();

    BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();

    m_commandMenuActive = false;
    m_messageActive = false;

    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage( kMessageUIClosed, m_pOwner );
}

// -------------------------------------------------------------
// Message Box
// -------------------------------------------------------------
void InteractionComponent::ShowActionMessage(const std::string& text)
{
    if (m_messageActive)
        return;
    m_messageActive = true;

    m_pMessageBackgroundNode->RemoveAllChildNodes();

    BB_FRectangle txtRect{ 16, 8, kMessageBoxWidth - 16, kMessageBoxHeight - 16};
    InterfaceText::Paremeters params
    {
        .pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf",
        .pText = text.c_str(),
        .textSize = kStandardUITextSize,
        .color = ColorPresets::white,
    };
    m_pMessageBackgroundNode->MakeChildNode<InterfaceText>( "message_log_text", txtRect, params );
    
    m_messageRootInterface.AddToScreen();
    BlackBoxManager::Get()->m_pInputManager->SwapInputTargetToInterface( &m_messageRootInterface );
    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage( kMessageUIOpen, m_pOwner );
}

void InteractionComponent::DismissActionMessage()
{
    if (!m_messageActive) 
        return;

    m_pMessageBackgroundNode->RemoveAllChildNodes();
    m_messageRootInterface.RemoveFromScreen();

    if ( m_commandMenuActive )
        BlackBoxManager::Get()->m_pInputManager->SwapInputTargetToInterface( &m_pCommandMenuRootNode );
    else
        BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();

    m_messageActive = false;
    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage( kMessageUIClosed, m_pOwner );
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
        ShowActionMessage("\'Unknown action.\'");
}

void InteractionComponent::HandleTalk()
{
    if (m_pCurrentTalk)
    {
        

        m_pCurrentTalk->OnTalkUsed(m_pOwner);
        if (m_pCurrentTalk->GetValue() == true)
        {
            ShowActionMessage("\'Guard: 'You may enter, my lord.' YOU WIN\'");
            //pInventory->SetHasTablet(false);

            //TEMP ->WHEN RESTARTING USED GAME DONE SO NO MORE TABLET?
            auto* pInventory = m_pOwner->GetComponent<InventoryComponent>();
            pInventory->SetHasTablet(false);
        }
        else
            ShowActionMessage("\'Guard: 'Halt! You need the Royal Pass.'\'");
    }
    else
        ShowActionMessage("\'There is no one there.\'");

}

void InteractionComponent::HandleStair()
{
    if (m_pCurrentStair)
    {
        CloseCommandUI();
        m_pCurrentStair->OnStairUsed(m_playerActor);
    }
    else
        ShowActionMessage("\'Thou canst not go down.\'");

}

void InteractionComponent::HandleTake()
{
    if (m_pCurrentTake)
    {
        m_pCurrentTake->OnTakeUsed(m_pOwner);
        ShowActionMessage("\'You found a tablet!\'");
    }
    else
        ShowActionMessage("\'There is nothing to take here.\'");

}

void InteractionComponent::OnLevelChanging()
{
    m_messageRootInterface.RemoveFromScreen();
    m_pCommandMenuRootNode.RemoveFromScreen();
    BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();
}

