#include "InteractionComponent.h"
#include "../Black Box Engine/Actors/ActorManager.h"
#include "../Black Box Engine/BlackBoxManager.h"
#include "../Black Box Engine/System/Delay.h"
#include "BlackBoxGame.h"
#include "PlayerMovementComponent.h"
#include "StairComponent.h"


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

void InteractionComponent::StartCombatUI(const std::string& enemyName, const std::string& spriteFile)
{
    //m_isCombatActive = true; //DONT FORGET TO SET IT TO FALSE WHEN RUN AWAY OR DIE OR WIN
    if (auto* playerMove = m_pOwner->GetComponent<PlayerMovementComponent>())
    { 
        playerMove->SetAnimationPaused(true); 
        playerMove->m_stopMoving = true;
    }

    // --- Enemy background and sprite ---
    BB_FRectangle bgRect{ 80.f, 80.f, 104.f, 74.f };
    InterfaceTexture::TextureInfo bgInfo{};
    bgInfo.pTextureFile = spriteFile.c_str();
    bgInfo.spriteDimensions = { 32, 32 };
    bgInfo.useFullImage = true;
    m_combatRoot.AddNode<InterfaceTexture>("Combat_BG", bgRect, bgInfo);

    // Enemy name & HP
    BB_FRectangle txtRect{ 104.f, 100.f, 100.f, 16.f };
    InterfaceText::Paremeters textParams{};
    textParams.pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf";
    textParams.textSize = 16;
    textParams.color = ColorValue{ 1, 1, 1, 1 };
    textParams.pText = enemyName.c_str(); //this should be in the text box 
    m_combatRoot.AddNode<InterfaceText>("EnemyName", txtRect, textParams);


      // --- Combat Panel ---
    constexpr float buttonW = 40.f;
    constexpr float buttonH = 16.f;
    constexpr float buttonSpacingX = 2.f;  
    constexpr float buttonSpacingY = 2.f;
    constexpr int kRows = 2;
    constexpr int kCols = 2;

    constexpr float panelPaddingX = 6.f;
    constexpr float panelPaddingY = 8.f;
    // Panel dimensions based on buttons
    BB_FRectangle panelRect{
        160.f, 20.f,
       kCols* buttonW + (kCols - 1) * buttonSpacingX + 2 * panelPaddingX,   // width
    kRows* buttonH + (kRows - 1) * buttonSpacingY + 2 * panelPaddingY   // height
    };

    InterfaceTexture::TextureInfo panelInfo{};
    panelInfo.pTextureFile = "../Assets/UI/SelectionBox.png";
    panelInfo.spriteDimensions = { 96, 72 };
    panelInfo.useFullImage = true;
    m_combatRoot.AddNode<InterfaceTexture>("CommandPanelBG", panelRect, panelInfo);

    // --- Combat Menu Buttons ---
    const char* actions[kRows * kCols] = { "Fight", "Magic", "Run", "Item" };
    InterfaceButton::ButtonParams btnParams{};
    btnParams.usable = true;
    btnParams.color = ColorValue{ 0, 0, 0, 0 };
    btnParams.targetedColor = ColorValue{ 0, 0, 0, 0 };
    btnParams.interactColor = ColorValue{ 0, 0, 0, 0 };

    float startX = panelRect.x + panelPaddingX;
    float startY = panelRect.y + panelPaddingY;

    InterfaceNode* nodes[kRows * kCols]{};

    // Create buttons in 2x2 grid
    for (int i = 0; i < kRows; ++i)
    {
        for (int j = 0; j < kCols; ++j)
        {
            int idx = i * kCols + j;
            BB_FRectangle buttonRect{
                startX + j * (buttonW + buttonSpacingX),
            startY + i * (buttonH + buttonSpacingY),
                buttonW,
                buttonH
            };

            btnParams.callbackFunction = [this, idx, actions]() {
                OnCombatButtonPressed(actions[idx]);
                };

            nodes[idx] = m_combatRoot.AddNode<InterfaceButton>(
                ("combat_btn_" + std::to_string(idx)).c_str(),
                buttonRect,
                btnParams
            );
        }
    }

    // --- Button Navigation (up/down/left/right) ---
    for (int i = 0; i < kRows; ++i)
    {
        for (int j = 0; j < kCols; ++j)
        {
            int idx = i * kCols + j;
            InterfaceNode* current = nodes[idx];

            int upIdx = ((i - 1 + kRows) % kRows) * kCols + j;
            int downIdx = ((i + 1) % kRows) * kCols + j;
            int leftIdx = i * kCols + ((j - 1 + kCols) % kCols);
            int rightIdx = i * kCols + ((j + 1) % kCols);

            current->SetAdjacentNode(Direction::kUp, nodes[upIdx]);
            current->SetAdjacentNode(Direction::kDown, nodes[downIdx]);
            current->SetAdjacentNode(Direction::kLeft, nodes[leftIdx]);
            current->SetAdjacentNode(Direction::kRight, nodes[rightIdx]);
        }
    }

    // --- Button Labels ---
    InterfaceText::Paremeters labelParams{};
    labelParams.pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf";
    labelParams.textSize = 16;
    labelParams.color = ColorValue{ 1, 1, 1, 1 };
    BB_FRectangle labelRect{ 4.f, 0.f, buttonW - 8.f, buttonH };

    for (int i = 0; i < kRows * kCols; ++i)
    {
        labelParams.pText = actions[i];
        nodes[i]->MakeChildNode<InterfaceText>(
            ("combat_btn_text_" + std::to_string(i)).c_str(),
            labelRect,
            labelParams
        );
    }

    // --- Highlight first button ---
    auto* highlighter = m_combatRoot.GetHighlight();
    highlighter->SetParameters({
        .mode = InterfaceHighlighter::kModeIcon,
        .pSpriteFile = "../Assets/UI/Icons/IconSpriteFile.xml",
        .iconOffset{-5, 0},
        .iconSize{4, 7}
        });
    highlighter->SetTarget(nodes[0]);
    m_combatRoot.SetCursorTarget(nodes[0]);

    // --- Add to screen ---
    m_combatRoot.AddToScreen();
    BlackBoxManager::Get()->m_pInputManager->SwapInputTargetToInterface(&m_combatRoot);

    //temp for the demo
    //auto* inputTarget = m_combatRoot.GetInputTarget();
    //inputTarget->m_keyDown.RegisterListener(KeyCode::kX, [this]() {
    //    EndCombatUI();
    //    if (auto* playerMove = m_pOwner->GetComponent<PlayerMovementComponent>()) {
    //        playerMove->SetAnimationPaused(false);
    //        playerMove->m_stopMoving = false;
    //
    //        BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();
    //    }
    //    });
}

void InteractionComponent::EndCombatUI()
{
    m_combatRoot.RemoveFromScreen();
    m_uiActive = false;
}

void InteractionComponent::OnCombatButtonPressed(const std::string& action)
{
    if (action == "Fight")
    {
        // Call attack logic
    }
    else if (action == "Magic")
    {
        // Cast spell
    }
    else if (action == "Item")
    {
        // Open inventory
    }

    //THIS TEMPS
    else if (action == "Run")
    {
        // End encounter / flee

        EndCombatUI();  // remove combat UI
        m_uiActive = false;  // allow player movement
        if (auto* playerMove = m_pOwner->GetComponent<PlayerMovementComponent>())
        {
            playerMove->SetAnimationPaused(false);  // resume animations
            playerMove->m_stopMoving = false;       // allow movement
        }

        BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();
    }
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