#include "InteractionComponent.h"
#include "../Black Box Engine/Actors/ActorManager.h"
#include "../Black Box Engine/BlackBoxManager.h"
#include "BlackBoxGame.h"
#include "StairComponent.h"
#include "PlayerMovementComponent.h"

using namespace BlackBoxEngine;

void ButtonOneCallback()
{
    BB_LOG(LogType::kMessage, "Button 1 clicked");
}
void ButtonTwoCallback()
{
    BB_LOG(LogType::kMessage, "Button 2 clicked");
}
void ButtonThreeCallback()
{
    BB_LOG(LogType::kMessage, "Button 3 clicked");
}
void ButtonFourCallback()
{
    BB_LOG(LogType::kMessage, "Button 4 clicked");
}

InteractionComponent::~InteractionComponent()
{
    auto* pInputManager = BlackBoxManager::Get()->m_pInputManager;
    using enum InputManager::InputType;

    // Unsubscribe all key downs
    for (auto id : m_keyDownCodes)
        pInputManager->UnsubscribeKey(id, kKeyDown);
}

void InteractionComponent::Start()
{
    auto* pInput = BlackBoxManager::Get()->m_pInputManager;
    using enum InputManager::InputType;
    
    // X = open UI
    m_keyDownCodes.emplace_back(
        pInput->SubscribeToKey(KeyCode::kX, kKeyDown, [this]() { if (!m_uiActive) OpenUI(); })
    );


    // Z = close UI
    m_keyDownCodes.emplace_back(
        pInput->SubscribeToKey(KeyCode::kZ, kKeyDown, [this]() { if (m_uiActive) CloseUI(); })
    );
    
    // Escape = also close UI
    m_keyDownCodes.emplace_back(
        pInput->SubscribeToKey(KeyCode::kEscape, kKeyDown, [this]() { if (m_uiActive) CloseUI(); })
    );
}

void InteractionComponent::OpenUI()
{
    TestInterfaceStuff();
    m_uiActive = true;

    auto* pPlayer = m_pOwner->GetComponent<PlayerMovementComponent>();

    if (pPlayer)
            pPlayer->SetAnimationPaused(true);
    

}

void InteractionComponent::CloseUI()
{
    m_interfaceRoot.RemoveFromScreen();
    m_messageRoot.RemoveFromScreen();

    BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();
    m_uiActive = false;
    m_messageActive = false;
    m_messageNode = nullptr;


    auto* pPlayer = m_pOwner->GetComponent<PlayerMovementComponent>();

    if (pPlayer)
        pPlayer->SetAnimationPaused(false);
    
}

void InteractionComponent::ShowActionMessage(const std::string& text)
{

    if (m_messageActive)
        return;
    
    m_messageActive = true;

    BB_FRectangle rect;
    rect.x = 50; rect.y = 50; rect.w = 200; rect.h = 20;
    
    InterfaceText::Paremeters params;
    params.pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf";
    params.textSize = 12;
    params.color = ColorPresets::white;
    params.pText = text.c_str();
    
    // create node on the *message* root and show only that root
    m_messageNode = m_messageRoot.AddNode<InterfaceText>("ActionMessage", rect, params);
    
    // make it visible
    m_messageRoot.AddToScreen();
    

}

void InteractionComponent::DismissActionMessage()
{
   
    if (!m_messageActive)
        return;

    // Hide the entire message root
    m_messageRoot.RemoveFromScreen();

    // Reset flags
    m_messageNode = nullptr;
    m_messageActive = false;
}

void InteractionComponent::Update()
{

    auto* pPlayer = m_pOwner->GetComponent<PlayerMovementComponent>();
    if (!pPlayer)
        return;

    bool isMoving = pPlayer->m_isMoving; // Assuming this exists, or you track input velocity

    if (!isMoving && !m_hudVisible)
    {
        DisplayHUD();
    }
    else if (isMoving && m_hudVisible)
    {
        HideHUD();
    }
}

void InteractionComponent::Render()
{
}

void InteractionComponent::OnCollide([[maybe_unused]]BlackBoxEngine::Actor* pOtherActor)
{
    if (!pOtherActor) return;

    // Check if the actor has a StairComponent
    if (auto* stair = pOtherActor->GetComponent<CaveEntranceComponent>())
    {
        m_pCurrentStair = stair;  // store the stair we collided with
        BB_LOG(LogType::kMessage, "Player is on a cave entrance actor.");
        return;
    }
    if (auto* stair1 = pOtherActor->GetComponent<StairUpLevel1Component>())
    {
        m_pCurrentStair = stair1;  // store the stair we collided with
        BB_LOG(LogType::kMessage, "Player is on a cave exit actor.");
        return;
    }
    if (auto* stair2 = pOtherActor->GetComponent<StairUpLevel2Component>())
    {
        m_pCurrentStair = stair2;  // store the stair we collided with
        BB_LOG(LogType::kMessage, "Player is on a cave level 1 actor.");
        return;
    }
    if (auto* stair3 = pOtherActor->GetComponent<StairDownComponent>())
    {
        m_pCurrentStair = stair3;  // store the stair we collided with
        BB_LOG(LogType::kMessage, "Player is on a cave level 2 actor.");
        return;
    }

    if (auto* talk = pOtherActor->GetComponent<TalkComponent>())
    {
        m_pCurrentTalk = talk;  // store the stair we collided with
        BB_LOG(LogType::kMessage, "Player is the town.");
        return;
    }

    if (auto* take = pOtherActor->GetComponent<TakeComponent>())
    {
        m_pCurrentTake = take;  // store the stair we collided with
        BB_LOG(LogType::kMessage, "Player is the chest.");
        return;
    }

    // Not a stair
    m_pCurrentStair = nullptr;
    m_pCurrentTalk = nullptr;

}

void InteractionComponent::Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser)
{
}

void InteractionComponent::Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser)
{
}

void InteractionComponent::OnButtonPressed(const std::string& action)
{
    if (action == "stair")
    {
        if (m_pCurrentStair)
        {
            CloseUI();

            //SetOwnerPosition(m_currentStair->GetTargetPosition());
            m_didMove = true;
            m_pCurrentStair->OnStairUsed(m_playerActor);
            return;
        }
        else
            ShowActionMessage("No Stairs To Take"); //need to check actual text 
    }

    if (action == "talk")
    {
        if (m_pCurrentTalk)
        {
            CloseUI();

            //SetOwnerPosition(m_currentStair->GetTargetPosition());
            m_didMove = true;
            m_pCurrentTalk->OnTalkUsed(m_playerActor);
            return;
        }
        else
            ShowActionMessage("No tile To Talk"); //need to check actual text 
    }

    if (action == "take")
    {
        if (m_pCurrentTake)
        {
            CloseUI();

            //SetOwnerPosition(m_currentStair->GetTargetPosition());
            m_didMove = true;
            m_pCurrentTake->OnTakeUsed(m_playerActor);
            return;
        }
        else
            ShowActionMessage("No tile To take"); //need to check actual text 
    }

    m_didMove = false;
    //ShowActionMessage("Cannot perform this action here.");
}



void InteractionComponent::DisplayHUD()
{

    if (m_hudVisible)
        return;

    m_hudVisible = true;

    BB_FRectangle rect;
    rect.w = 50;
    rect.h = 70;
    rect.x = 10;
    rect.y = 10;

    //BB_FRectangle rect;
    //rect.w = 150;  // widened for label + value
    //rect.h = 120;
    //rect.x = 10;
    //rect.y = 10;

    // --- Add background texture ---
    InterfaceTexture::TextureInfo bgTextureInfo{
        .pTextureFile = "../Assets/UI/StatsInfoBox.png",
        .spriteDimensions = {16, 16},  // full size of your texture
        .useFullImage = true,
        .animate = false
    };

    m_hudRoot.AddNode<InterfaceTexture>("HUD_Background", rect, bgTextureInfo);

    // --- Add header text ---
    InterfaceText::Paremeters headerParams;
    headerParams.pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf";
    headerParams.textSize = 14;
    headerParams.color = ColorPresets::white;
    headerParams.pText = "     PLAYER";

    BB_FRectangle headerRect = rect;
    headerRect.h = 15;
    //headerRect.y += 2; // small top padding
    m_hudRoot.AddNode<InterfaceText>("HUD_Header", headerRect, headerParams);

    // --- Add stats text ---
    BB_FRectangle statsRect = rect;
    statsRect.y += 12;
    
    std::string hudText =
        "  LV       " + std::to_string(m_playerLevel) + "\n\n" +
        "  HP       " + std::to_string(m_playerHP) + "\n\n" +
        "  MP       " + std::to_string(m_playerMP) + "\n\n" +
        "  G        " + std::to_string(m_playerGold) + "\n\n" +
        "  E        " + std::to_string(m_playerEnergy);
    
    InterfaceText::Paremeters statsParams;
    statsParams.pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf";
    statsParams.textSize = 16;
    statsParams.color = ColorPresets::white;
    statsParams.pText = hudText.c_str(); 

    m_hudRoot.AddNode<InterfaceText>("HUD_Stats", statsRect, statsParams);
    // --- Show HUD ---
    m_hudRoot.AddToScreen();
}



void InteractionComponent::TestInterfaceStuff()
{
    using enum Direction;
    using namespace BlackBoxEngine;

    m_interfaceRoot.GetRoot()->SetOffset(20, 20);

    auto* pHighlighter = m_interfaceRoot.GetHighlight();
    pHighlighter->SetParameters({
        .mode = InterfaceHighlighter::kModeIcon,
        .pSpriteFile = "../Assets/UI/Icons/IconSpriteFile.xml",
        .iconOffset{-5, -2},
        .iconSize{ 4,7 }
        });

   
    //m_interfaceRoot.AddNode<InterfaceTexture>( "TextureTest", {50,0,16,16}, InterfaceTexture::TextureInfo{
    //    .pTextureFile = "../Assets/Sprites/Player/Fixed_OffsetCharacterSheet.png",
    //    .spriteDimensions = {16,16},
    //    .useFullImage = false,
    //    .animate = true,
    //    .spriteXCount = 8,
    //    .spriteYCount = 1,
    //    .spriteXPad = 0,
    //    .spriteYPad = 0,
    //    .animationStartIndex = 0,
    //    .animationEndIndex = 7,
    //    .spriteSheetIndex = 0,
    //    .framesPerSecond = 2,
    //    .repeat = true
    //} );


    BB_FRectangle buttonDimension{ 0,0, 64, 7 };

    InterfaceButton::ButtonParams buttomParams{
        .usable = true,
        .color = ColorValue(0,0,0,0),
        .targetedColor = ColorValue(0,0,0,0),
        .interactColor = ColorValue(0,0,0,0),
    };

    std::string buttonNames[] = { "Talk", "Stair", "Take", "Item" };
    std::string actions[] = { "talk", "stair", "take", "item" };

   ButtonCallbackFunctionPtr callbacks[] =
   {
       [this]() { OnButtonPressed("talk"); }, // First button
       [this]() { OnButtonPressed("stair"); },  // Second button
       [this]() { OnButtonPressed("take"); }, // Third button
       [this]() { OnButtonPressed("item"); }   // Fourth button
   };

    InterfaceNode* m_nodes[4] = {};

    float yPad = 1;
    size_t buttonCount = std::size(callbacks);
    for (size_t i = 0; i < buttonCount; ++i)
    {
        std::string name = "button_" + std::to_string(i);
        //buttomParams.callbackFunction = callbacks[i];
        buttonDimension.y = i * (buttonDimension.h + yPad);

        buttomParams.callbackFunction = [this, i]() {
            const char* actions[] = { "talk", "stair", "take", "item" };
            OnButtonPressed(actions[i]);
            };

        auto* pButton = m_interfaceRoot.AddNode<InterfaceButton>(name.c_str(), buttonDimension, buttomParams);
        m_nodes[i] = pButton;
    
        if (i == 0)
            continue;
        pButton->SetAdjacentNode(kUp, m_nodes[i - 1]);
        m_nodes[i - 1]->SetAdjacentNode(kDown, pButton);
    }


    m_nodes[3]->SetAdjacentNode(kDown, m_nodes[0]);
    m_nodes[0]->SetAdjacentNode(kUp, m_nodes[3]);

    buttonDimension.y = 0;
    buttonDimension.x = 0;

    InterfaceText::Paremeters textParams{
        .pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf",
        .textSize = 12,
        .color = ColorPresets::white
    };

    textParams.pText = "Talk";
    m_nodes[0]->MakeChildNode<InterfaceText>("FirstButton Text", buttonDimension, textParams);

    textParams.pText = "Stair";
    m_nodes[1]->MakeChildNode<InterfaceText>("SecondButton Text", buttonDimension, textParams);

    textParams.pText = "Take";
    m_nodes[2]->MakeChildNode<InterfaceText>("ThirdButton Text", buttonDimension, textParams);

    textParams.pText = "Item";
    m_nodes[3]->MakeChildNode<InterfaceText>("FourthButton Text", buttonDimension, textParams);


    pHighlighter->SetTarget(m_nodes[0]);

    m_interfaceRoot.SetCursorTarget(m_nodes[0]); // this will crash if you forget

    m_interfaceRoot.AddToScreen();

    //BlackBoxManager::Get()->m_pInputManager->SwapInputTargetToInterface(&m_interfaceRoot);
    auto* pInput = BlackBoxManager::Get()->m_pInputManager;
    pInput->SwapInputTargetToInterface(&m_interfaceRoot);
    
    // Bind Z/Escape to close UI (do not touch arrow keys)
    auto* pInterfaceTarget = m_interfaceRoot.GetInputTarget();
    pInterfaceTarget->m_keyDown.RegisterListener(KeyCode::kZ, [this]() { CloseUI(); });
    pInterfaceTarget->m_keyDown.RegisterListener(KeyCode::kEscape, [this]() { CloseUI(); });
    
    pInterfaceTarget->m_keyDown.RegisterListener(KeyCode::kX, [this]() {
        if (m_messageActive)
            DismissActionMessage();
        else if (!m_uiActive)
            OpenUI(); // fallback if needed
        });


    m_uiActive = true;
}


void InteractionComponent::HideHUD()
{
    if (!m_hudVisible)
        return;

    m_hudRoot.RemoveFromScreen();
    m_hudTextNode = nullptr;
    m_hudVisible = false;
}