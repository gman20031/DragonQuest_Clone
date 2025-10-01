#include "InteractionComponent.h"
#include "../Black Box Engine/Actors/ActorManager.h"
#include "../Black Box Engine/BlackBoxManager.h"
#include "BlackBoxGame.h"
#include "CaveEntranceComponent.h"

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
}

void InteractionComponent::CloseUI()
{
    m_interfaceRoot.RemoveFromScreen();
    BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();
    m_uiActive = false;

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

    // Check if the actor is interactable in any way
    if (pOtherActor->GetComponent<CaveEntranceComponent>())
    {
        m_currentActor = pOtherActor; // store the actor the player is colliding with
        //BB_LOG(LogType::kMessage, "Player is now colliding with actor: %s", pOtherActor->GetId().c_str());
    }
    else
    {
        m_currentActor = nullptr; // not colliding with anything relevant
    }
}

void InteractionComponent::Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser)
{
}

void InteractionComponent::Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser)
{
}

void InteractionComponent::OnButtonPressed(const std::string& action)
{
    if (!m_currentActor)
    {
        BB_LOG(LogType::kMessage, "No actor to interact with.");
        return;
    }

    // Example using the action string
    if (action == "stair")
    {
        if (auto* caveComp = m_currentActor->GetComponent<CaveEntranceComponent>())
        {
            caveComp->OnInteract();
            return;
        }
    }
   //else if (action == "talk")
   //{
   //    if (auto* townComp = m_currentActor->GetComponent<TownNPCComponent>())
   //    {
   //        townComp->OnInteract();
   //        return;
   //    }
   //}

    BB_LOG(LogType::kMessage, "Cannot perform this action here.");
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

    std::string buttonNames[] = { "Talk", "Stair", "Trade", "Look" };
    std::string actions[] = { "talk", "stair", "trade", "look" };

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
       [this]() { OnButtonPressed("trade"); }, // Third button
       [this]() { OnButtonPressed("look"); }   // Fourth button
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
            const char* actions[] = { "talk", "stair", "trade", "look" };
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

    textParams.pText = "Third Button";
    m_nodes[2]->MakeChildNode<InterfaceText>("ThirdButton Text", buttonDimension, textParams);

    textParams.pText = "Fourth Button";
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
    
    m_uiActive = true;
}

