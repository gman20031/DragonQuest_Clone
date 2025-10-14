#include "InteractionComponent.h"
#include "../Black Box Engine/Actors/ActorManager.h"
#include "../Black Box Engine/BlackBoxManager.h"
#include "../Black Box Engine/System/Delay.h"
#include "BlackBoxGame.h"
#include "PlayerMovementComponent.h"
#include "StairComponent.h"
#include "TileSystem/EncounterComponent.h"


using namespace BlackBoxEngine;

InteractionComponent::~InteractionComponent()
{
    auto* input = BlackBoxManager::Get()->m_pInputManager;
    using enum InputManager::InputType;
    for (auto id : m_keyDownCodes)
        input->UnsubscribeKey(id, kKeyDown);
}

// -------------------------------------------------------------
// Start
// -------------------------------------------------------------
void InteractionComponent::Start()
{
    auto* input = BlackBoxManager::Get()->m_pInputManager;
    using enum InputManager::InputType;

    // X = open UI
    m_keyDownCodes.emplace_back(
        input->SubscribeToKey(KeyCode::kX, kKeyDown, [this]() { if (!m_uiActive) OpenUI(); })
    );

    // Z = close UI
    m_keyDownCodes.emplace_back(
        input->SubscribeToKey(KeyCode::kZ, kKeyDown, [this]() { if (m_uiActive) CloseUI(); })
    );
}

// -------------------------------------------------------------
// Update
// -------------------------------------------------------------
void InteractionComponent::Update()
{
    auto* playerMove = m_pOwner->GetComponent<PlayerMovementComponent>();
    if (!playerMove) return;

    bool isMoving = playerMove->m_isMoving;

    // Show HUD after player stops
    if (!isMoving && !m_hudVisible && !m_uiActive && !m_isChangingLevel)
    {
        if (m_delayedDisplayId == 0)
        {
            m_delayedDisplayId = DelayedCallbackManager::AddCallback([this]()
                {
                    auto* pm = m_pOwner->GetComponent<PlayerMovementComponent>();
                    if (pm && !pm->m_isMoving && !m_hudVisible && !m_uiActive && !m_isChangingLevel)
                        DisplayHUD();

                    m_delayedDisplayId = 0;
                }, 1000);
        }
    }
    // Hide HUD if player moves or UI active
    else if ((isMoving || m_uiActive) && m_hudVisible && !m_isChangingLevel)
    {
        HideHUD();

        if (m_delayedDisplayId != 0)
        {
            DelayedCallbackManager::RemoveCallback(m_delayedDisplayId);
            m_delayedDisplayId = 0;
        }
    }
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
    if (m_hudVisible)
        HideHUD();

    SelectionMenu();
    m_uiActive = true;

    if (auto* player = m_pOwner->GetComponent<PlayerMovementComponent>())
        player->SetAnimationPaused(true);
}

void InteractionComponent::CloseUI()
{
    m_interfaceRoot.RemoveFromScreen();
    m_messageRoot.RemoveFromScreen();

    BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();

    m_uiActive = false;
    m_messageActive = false;
    m_messageNode = nullptr;

    if (auto* player = m_pOwner->GetComponent<PlayerMovementComponent>())
        player->SetAnimationPaused(false);
}

// -------------------------------------------------------------
// UI: Selection Menu
// -------------------------------------------------------------
void InteractionComponent::SelectionMenu()
{
    using namespace BlackBoxEngine;
    using enum Direction;

    constexpr float kButtonW = 64.f;
    constexpr float kButtonH = 7.f;
    constexpr float kYPad = 1.f;
    constexpr int kButtonCount = 4;

    // Offset menu
    m_interfaceRoot.GetRoot()->SetOffset(20, 20);

    // Highlighter
    auto* highlighter = m_interfaceRoot.GetHighlight();
    highlighter->SetParameters({
        .mode = InterfaceHighlighter::kModeIcon,
        .pSpriteFile = "../Assets/UI/Icons/IconSpriteFile.xml",
        .iconOffset{-5, -2},
        .iconSize{4, 7}
        });

    // Background box
    BB_FRectangle bgRect{ -5, -5, kButtonW, (kButtonH + kYPad) * kButtonCount + 10 };
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/SelectionBox.png",
        .spriteDimensions = {16, 16},
        .useFullImage = true
    };
    m_interfaceRoot.AddNode<InterfaceTexture>("UI_Background", bgRect, bgInfo);

    // Button setup
    InterfaceButton::ButtonParams btnParams{
        .usable = true,
        .color = ColorValue(0, 0, 0, 0),
        .targetedColor = ColorValue(0, 0, 0, 0),
        .interactColor = ColorValue(0, 0, 0, 0)
    };

    const char* actions[kButtonCount] = { "talk", "stair", "take", "item" };
    InterfaceNode* nodes[kButtonCount] = {};

    BB_FRectangle rect{ 0, 0, kButtonW, kButtonH };

    for (int i = 0; i < kButtonCount; ++i)
    {
        rect.y = i * (kButtonH + kYPad);
        btnParams.callbackFunction = [this, i, actions]() { OnButtonPressed(actions[i]); };

        auto* btn = m_interfaceRoot.AddNode<InterfaceButton>(
            ("button_" + std::to_string(i)).c_str(), rect, btnParams
        );
        nodes[i] = btn;

        if (i > 0)
        {
            btn->SetAdjacentNode(kUp, nodes[i - 1]);
            nodes[i - 1]->SetAdjacentNode(kDown, btn);
        }
    }

    // Wrap navigation
    nodes[0]->SetAdjacentNode(kUp, nodes[kButtonCount - 1]);
    nodes[kButtonCount - 1]->SetAdjacentNode(kDown, nodes[0]);

    // Add text labels
    InterfaceText::Paremeters textParams{
        .pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf",
        .textSize = 16,
        .color = ColorPresets::white
    };

    const char* labels[kButtonCount] = { "Talk", "Stair", "Take", "Item" };
    rect.x = rect.y = 0;
    for (int i = 0; i < kButtonCount; ++i)
    {
        textParams.pText = labels[i];
        nodes[i]->MakeChildNode<InterfaceText>(
            ("button_text_" + std::to_string(i)).c_str(), rect, textParams
        );
    }

    highlighter->SetTarget(nodes[0]);
    m_interfaceRoot.SetCursorTarget(nodes[0]);
    m_interfaceRoot.AddToScreen();

    auto* input = BlackBoxManager::Get()->m_pInputManager;
    input->SwapInputTargetToInterface(&m_interfaceRoot);

    auto* inputTarget = m_interfaceRoot.GetInputTarget();
    inputTarget->m_keyDown.RegisterListener(KeyCode::kZ, [this]() { CloseUI(); });
    inputTarget->m_keyDown.RegisterListener(KeyCode::kX, [this]() {
        if (m_messageActive) DismissActionMessage();
        else if (!m_uiActive) OpenUI();
        });
}

// -------------------------------------------------------------
// HUD
// -------------------------------------------------------------
void InteractionComponent::DisplayHUD()
{
    if (m_hudVisible) return;

    m_hudVisible = true;

    constexpr BB_FRectangle bgRect{ 10, 10, 50, 70 };
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/StatsInfoBox.png",
        .spriteDimensions = {16, 16},
        .useFullImage = true
    };
    m_hudRoot.AddNode<InterfaceTexture>("HUD_Background", bgRect, bgInfo);

    InterfaceText::Paremeters textParams{
        .pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf",
        .textSize = 16,
        .color = ColorPresets::white
    };

    // Header
    BB_FRectangle headerRect = bgRect;
    headerRect.h = 15;
    textParams.pText = "     PLAYER";
    m_hudRoot.AddNode<InterfaceText>("HUD_Header", headerRect, textParams);

    // Stats
    BB_FRectangle statsRect = bgRect;
    statsRect.y += 12;
    std::string stats =
        "  LV       " + std::to_string(m_playerLevel) + "\n\n" +
        "  HP       " + std::to_string(m_playerHP) + "\n\n" +
        "  MP       " + std::to_string(m_playerMP) + "\n\n" +
        "  G        " + std::to_string(m_playerGold) + "\n\n" +
        "  E        " + std::to_string(m_playerEnergy);
    textParams.pText = stats.c_str();
    m_hudRoot.AddNode<InterfaceText>("HUD_Stats", statsRect, textParams);

    m_hudRoot.AddToScreen();
}

void InteractionComponent::HideHUD()
{
    if (!m_hudVisible) return;
    m_hudRoot.RemoveFromScreen();
    m_hudVisible = false;
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
    {
        //CloseUI();
        //m_didMove = true;
        ShowActionMessage("Trading with Inn.");
    }
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
    m_isChangingLevel = true;
    if (m_hudVisible) HideHUD();

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