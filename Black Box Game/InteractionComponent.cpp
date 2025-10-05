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
        m_currentStair = stair;  // store the stair we collided with
        BB_LOG(LogType::kMessage, "Player is on a cave entrance actor.");
        return;
    }
    if (auto* stair1 = pOtherActor->GetComponent<StairUpLevel1Component>())
    {
        m_currentStair = stair1;  // store the stair we collided with
        BB_LOG(LogType::kMessage, "Player is on a cave exit actor.");
        return;
    }
    if (auto* stair2 = pOtherActor->GetComponent<StairUpLevel2Component>())
    {
        m_currentStair = stair2;  // store the stair we collided with
        BB_LOG(LogType::kMessage, "Player is on a cave level 1 actor.");
        return;
    }
    if (auto* stair3 = pOtherActor->GetComponent<StairDownComponent>())
    {
        m_currentStair = stair3;  // store the stair we collided with
        BB_LOG(LogType::kMessage, "Player is on a cave level 2 actor.");
        return;
    }
    // Not a stair
    m_currentStair = nullptr;


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
        if (m_currentStair)
        {
            CloseUI();

            //SetOwnerPosition(m_currentStair->GetTargetPosition());
            m_didMove = true;
            m_currentStair->OnStairUsed(m_playerActor);
            return;
        }
        else
            ShowActionMessage("No Stairs To Take"); //need to check actual text 
    }

    m_didMove = false;
    //ShowActionMessage("Cannot perform this action here.");
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

    //ButtonCallbackFunctionPtr callbacks[] =
    //{
    //    &ButtonOneCallback,
    //    &ButtonTwoCallback,
    //    &ButtonThreeCallback,
    //    &ButtonFourCallback
    //};

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

