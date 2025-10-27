#include "InteractionComponent.h"

#include <Actors/ActorManager.h>
#include <BlackBoxManager.h>
#include <System/Delay.h>
#include <Graphics/ScreenFader.h>

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

static constexpr float kStandardUITextSize = 26.f;

static constexpr float kItemBoxStartX = 10 * 16;  // right side of the screen
static constexpr float kItemBoxStartY = 3 * 16;

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

    good = CreateItemBox();
    if (!good) { BB_LOG(LogType::kError, "Failed to create item menu ui"); }
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

    // encounter starts, then we need to make sure this ui closes 
    m_messageIds.emplace_back(
        BlackBoxManager::Get()->m_pMessagingManager->RegisterListener
        (
            kEncounterStarted, [this]( [[maybe_unused]] Message& )
            {
                if ( m_commandMenuActive )
                    CloseCommandUI();
            }
        ) );

    m_messageIds.emplace_back(
    BlackBoxManager::Get()->m_pMessagingManager->RegisterListener( 
        kLevelChanging, [this]( [[maybe_unused]]Message& ) {OnLevelChanging();} 
    ));

    m_pCurrentTalk = nullptr;
}

void InteractionComponent::Update()
{
    if (m_activeScrollBox)
        m_activeScrollBox->Update();
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
        .callbackFunction = [this]() {
            // Only dismiss if the text has finished scrolling
            if (!m_activeScrollBox || !m_activeScrollBox->IsAnimating())
            {
                DismissActionMessage();
            }
        }
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
void InteractionComponent::ShowActionMessage(const std::string& text )
{
    if (m_messageActive)
        return;
    m_messageActive = true;
    
    
    m_pMessageBackgroundNode->RemoveAllChildNodes();

    BB_FRectangle txtRect{ 16, 8, kMessageBoxWidth - 16, kMessageBoxHeight - 16};
    BlackBoxEngine::ScrollingTextBox::Params params;
    params.charsPerSecond = 40.f;
    params.scrollSpeed = 40.f;

    m_activeScrollBox = m_pMessageBackgroundNode->MakeChildNode<BlackBoxEngine::ScrollingTextBox>("scrolling_text_box", txtRect, params);
    m_activeScrollBox->SetText(text);

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

    m_activeScrollBox = nullptr;
    if (m_messageFromItemMenu)
    {
        // Close item menu only when X pressed
        CloseItemMenu();
        m_messageFromItemMenu = false;

        // Return control to command menu if active
        if (m_commandMenuActive)
            BlackBoxManager::Get()->m_pInputManager->SwapInputTargetToInterface(&m_pCommandMenuRootNode);
        else
            BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();
    }
    else if (m_itemMenuActive)
    {
        BlackBoxManager::Get()->m_pInputManager->SwapInputTargetToInterface(&m_itemMenuInterface);
    }
    else if (m_commandMenuActive)
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
    BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/32DragonQuest1-MenuButton.wav", 0.2f);

    if (m_activeScrollBox && m_activeScrollBox->IsAnimating())
        return;

    static const std::unordered_map<std::string, void(InteractionComponent::*)()> kActionMap = {
        {"talk", &InteractionComponent::HandleTalk},
        {"stair", &InteractionComponent::HandleStair},
        {"take", &InteractionComponent::HandleTake},
        {"item", &InteractionComponent::HandleItem},
    };

    auto it = kActionMap.find(action);
    if (it != kActionMap.end())
        (this->*(it->second))();
    else
        ShowActionMessage("\'Unknown action.\'");
}

void InteractionComponent::HandleTalk()
{

    auto* pInventory = m_pOwner->GetComponent<InventoryComponent>();

    if (!m_pCurrentTalk)
    {
        ShowActionMessage("'There is no one here.'");
        return;
    }

    else if(m_pCurrentTalk == m_pCurrentTalk->GetOwner()->GetComponent<CastleTalkComponent>())
    {
        if (pInventory->GetHasTablet())
        {
            BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/25DragonQuest1-Victory.wav", 0.4f);
            ShowActionMessage("'Guard: \n'You may enter, my lord.' YOU WIN'");
        }
        else
            ShowActionMessage("'Guard: \n'Halt! You need the Royal Pass.'");
    }

    else if (m_pCurrentTalk == m_pCurrentTalk->GetOwner()->GetComponent<InnTalkComponent>())
    {
        ShowActionMessage("'Welcome to the \ntraverler's Inn.\n\nRoom and board is \n6 Gold per night.\n\nDost thou want \na room?'");

        BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/33DragonQuest1-Confirmation.wav", 0.2f);

        std::vector<std::pair<std::string, int>> choices = { {"Yes", 0}, {"No", 0} };
        ShowChoiceMenu(choices, [this](const std::string& choice) {
            

            if (choice == "Yes")
            {
                BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/32DragonQuest1-MenuButton.wav", 0.2f);
                DismissActionMessage();
                ShowActionMessage("'Good night.'");
                DelayedCallbackManager::AddCallback([this]()
                    {
                        SleepAtInn();
                    }, std::chrono::milliseconds(1000));
            }
            else
            {
                BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/32DragonQuest1-MenuButton.wav", 0.2f);
                
                ShowActionMessage("'Okay.\nGood-bye, traveler.'");
                DismissChoiceMenu();
                DismissActionMessage();
            }
            });
    }

}

void InteractionComponent::HandleStair()
{
    if (m_pCurrentStair)
    {
        CloseCommandUI();
        m_pCurrentStair->OnStairUsed(m_playerActor);
    }
    else
        ShowActionMessage("\'There are no stair here.\'");

}

void InteractionComponent::HandleTake()
{
    if (m_pCurrentTake)
    {
        m_pCurrentTake->OnTakeUsed(m_pOwner);
        ShowActionMessage("\'Fortune smiles \nupon thee. \nThou hast found the Tablet.\'");
    }
    else
        ShowActionMessage("\'There is nothing \nto take here.\'");

}

void InteractionComponent::HandleItem()
{

    auto* pInventory = m_pOwner->GetComponent<InventoryComponent>();
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

void InteractionComponent::OnLevelChanging()
{
    m_messageRootInterface.RemoveFromScreen();
    m_pCommandMenuRootNode.RemoveFromScreen();
    BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();
}


bool InteractionComponent::CreateItemBox()
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

void InteractionComponent::ShowItemMenu(const std::vector<std::pair<std::string, int>>& items)
{
    if (m_itemMenuActive) return;
    m_itemMenuActive = true;

    // Clear previous nodes and storage
    m_pItemBackgroundNode->RemoveAllChildNodes();
    m_itemTextStorage.clear();

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

        // Button callback now only shows a message, does NOT close item menu
        btnParams.callbackFunction = [this, name]()
            {
                BlackBoxManager::Get()->m_pAudioManager->PlaySound("../Assets/Audio/32DragonQuest1-MenuButton.wav", 0.2f);

                auto* pInventory = m_pOwner->GetComponent<InventoryComponent>();
                if (!pInventory) return;

                std::string message;
                if (name == "Torch" && pInventory->GetHasTorch())
                {
                    message = std::format("'A torch can be \nused only in dark \nplaces.'", name);
                }
                else if (name == "Tablet" && pInventory->GetHasTablet())
                {
                    message = std::format("'Give thee to the Castle \nfor permission to enter!'", name);
                }
                else if(name == "Club" && pInventory->GetHasClub() || name == "Leather Armor" && (pInventory->GetHasLeatherClothes()))
                {
                    message = std::format("'Thou cannot use \nthe {} \nnow.'", name);
                }

                // Show the message **first** without closing the item menu yet
                m_messageFromItemMenu = true;  // indicate message came from item menu
                ShowActionMessage(message);
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

void InteractionComponent::CloseItemMenu()
{
    if (!m_itemMenuActive) return;

    m_pItemBackgroundNode->RemoveAllChildNodes();
    m_itemMenuInterface.RemoveFromScreen();
    m_itemMenuActive = false;

    // Return control to command menu
    if (m_commandMenuActive)
        BlackBoxManager::Get()->m_pInputManager->SwapInputTargetToInterface(&m_pCommandMenuRootNode);
    else
        BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();

    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage(kMessageUIClosed, m_pOwner);
}

void InteractionComponent::ShowChoiceMenu(
    const std::vector<std::pair<std::string, int>>& choices,
    std::function<void(const std::string&)> callback)
{
    if (m_choiceMenuActive) return;
    m_choiceMenuActive = true;

    m_choiceCallback = callback; // store callback

    m_pItemBackgroundNode->RemoveAllChildNodes();
    m_itemTextStorage.clear();

    static constexpr float kLineHeight = 20.f;
    static constexpr float kItemBoxWidth = 60.f;
    float totalHeight = choices.size() * kLineHeight + 8.f;

    constexpr float kChoiceBoxStartX = -60.f; // far left
    constexpr float kChoiceBoxStartY = -16.f; // top of the screen

    m_pItemBackgroundNode->SetOffset(kChoiceBoxStartX, kChoiceBoxStartY);
    m_pItemBackgroundNode->SetSize(kItemBoxWidth, totalHeight);

    // --- Background texture ---
    BB_FRectangle bgRect{ 0, 0, kItemBoxWidth, totalHeight };
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/Yes_No_Menu.png",
        .useFullImage = true
    };
    InterfaceNode* boxNode = m_pItemBackgroundNode->MakeChildNode<InterfaceTexture>("ChoiceMenu_BG", bgRect, bgInfo);

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

    std::vector<InterfaceNode*> buttons;
    for (int i = 0; i < choices.size(); ++i)
    {
        const auto& label = choices[i].first;
        constexpr float kPaddingX = 12.f;  // space from left/right
        constexpr float kPaddingY = 4.f;  // space from top/bottom
        constexpr float kLineSpacing = 2.f; // space between buttons

        BB_FRectangle btnRect{
            kPaddingX,
            kPaddingY + i * (kLineHeight + kLineSpacing), // vertical spacing
            kItemBoxWidth - 2 * kPaddingX,
            kLineHeight
        };
        std::string btnName = "ChoiceBtn_" + label;

        // Store label text
        m_itemTextStorage.push_back(label);
        std::string textPtr = m_itemTextStorage.back();

        btnParams.callbackFunction = [this, textPtr]() {
            if (m_choiceCallback) m_choiceCallback(textPtr);
            };

        InterfaceNode* btnNode = boxNode->MakeChildNode<InterfaceButton>(btnName.c_str(), btnRect, btnParams);

        BB_FRectangle txtRect{ 0, 0, btnRect.w, btnRect.h };
        textParams.pText = textPtr.c_str();
        btnNode->MakeChildNode<InterfaceText>((btnName + "_Label").c_str(), txtRect, textParams);

        buttons.push_back(btnNode);
    }

    // Link buttons
    for (int i = 0; i < buttons.size(); ++i)
    {
        if (i > 0) buttons[i]->SetAdjacentNode(Direction::kUp, buttons[i - 1]);
        if (i < buttons.size() - 1) buttons[i]->SetAdjacentNode(Direction::kDown, buttons[i + 1]);
    }

    // Highlight first button
    if (!buttons.empty())
    {
        m_itemMenuInterface.SetCursorTarget(buttons[0]);
        m_itemMenuInterface.GetHighlight()->SetTarget(buttons[0]);
    }

    m_itemMenuInterface.AddToScreen();
    BlackBoxManager::Get()->m_pInputManager->SwapInputTargetToInterface(&m_itemMenuInterface);
    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage(kMessageUIOpen, m_pOwner);
}

void InteractionComponent::SleepAtInn()
{
    DismissActionMessage();
    BlackBoxManager::Get()->m_pInputManager->StopAllInput();

    auto* pStats = m_pOwner->GetComponent<PlayerStatsComponent>();
    if (!pStats) return;

    if (pStats->GetPlayerGold() < 6)
    {
        ShowActionMessage("'Thou hast not enough gold.'");
        DismissChoiceMenu();
        return;
    }

    int gold = pStats->GetPlayerGold() - 6;

    pStats->SetPlayerGold(gold);
    pStats->SetPlayerHP(pStats->GetPlayerMaxHP());

    DismissChoiceMenu();

    ScreenFader::FadeToBlack(1.0f);

    // Step 2: Delay until fade is done
    auto fadeInAndShowMessage = [this]()
        {
            // Step 3: Fade back in
            ScreenFader::FadeIn(1.0f);

            // Step 4: Show the message after fade-in
            DelayedCallbackManager::AddCallback([this]()
                {
                    ShowActionMessage("'Good morning.\nThou seems to have spent a good night.\n\nI shall see \nthee again.'");
                }, std::chrono::milliseconds(1000)); // match fade-in duration
        };

    DelayedCallbackManager::AddCallback(fadeInAndShowMessage, std::chrono::milliseconds(1000)); // match fade-out duration
}

void InteractionComponent::DismissChoiceMenu()
{
    if (!m_choiceMenuActive) return;

    m_pItemBackgroundNode->RemoveAllChildNodes();
    m_itemMenuInterface.RemoveFromScreen();
    m_choiceMenuActive = false;
    m_choiceCallback = nullptr;

    // Return control to command menu or game
    if (m_commandMenuActive)
        BlackBoxManager::Get()->m_pInputManager->SwapInputTargetToInterface(&m_pCommandMenuRootNode);
    else
        BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();

    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage(kMessageUIClosed, m_pOwner);
}