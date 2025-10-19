#include "EncounterComponent.h"
#include <Resources/ResourceManager.h>

#include "EncounterHandler.h"
#include "../Black Box Engine/Actors/ActorManager.h"
#include "../Black Box Engine/BlackBoxManager.h"
#include "../Black Box Engine/Actors/EngineComponents/SpriteComponent.h"
#include "../Black Box Game/InteractionComponent.h"
#include "../Black Box Game/PlayerMovementComponent.h"
#include <format>

#include <Graphics/ScreenFader.h>
#include <Actors/EngineComponents/TransformComponent.h>
#include <System/Delay.h>

using namespace BlackBoxEngine;

void EncounterComponent::Start()
{
}

void EncounterComponent::StartEncounter(Actor* pOtherActor)
{
  

    m_inBattle = true;
    BB_LOG(LogType::kMessage, "Enemy '%s' appeared! HP=%d", m_name.c_str(), m_hp);

    SetPlayer(pOtherActor);
    StartCombatUI();
}

void EncounterComponent::EndEncounter()
{

    if (auto* playerMove = m_pPlayer->GetComponent<PlayerMovementComponent>())
    {
        playerMove->SetAnimationPaused(false);
        playerMove->m_stopMoving = false;
    }

    m_inBattle = false;
    BB_LOG(LogType::kMessage, "Encounter ended.");
}

float EncounterComponent::RandomFloat()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
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
    if (!pStats) return;

    float roll = RandomFloat();
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
    else if (m_name == "Magician")
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
        int roll = rand() % 64;
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
    else if (m_name == "Magician")
        fleeChance *= 0.9f; // Magicians may try to trap you with magic

    // --- Roll the outcome ---
    if (RandomFloat() < fleeChance)
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
        int damage = 8 + rand() % 6; // 8–13 damage
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
    if (auto* playerMove = m_pPlayer->GetComponent<PlayerMovementComponent>())
    {
        playerMove->SetAnimationPaused(true);
        playerMove->m_stopMoving = true;
    }

    // --- Enemy background and sprite ---
    BB_FRectangle bgRect{ 80.f, 80.f, 104.f, 74.f };
    InterfaceTexture::TextureInfo bgInfo{};
    bgInfo.pTextureFile = m_spriteFile.c_str();
    bgInfo.spriteDimensions = { 32, 32 };
    bgInfo.useFullImage = true;
    m_combatRoot.AddNode<InterfaceTexture>("Combat_BG", bgRect, bgInfo);

    // Enemy name & HP
    BB_FRectangle txtRect{ 104.f, 100.f, 100.f, 16.f };
    InterfaceText::Paremeters textParams{};
    textParams.pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf";
    textParams.textSize = 16;
    textParams.color = ColorValue{ 1, 1, 1, 1 };
    textParams.pText = m_name.c_str(); //this should be in the text box 
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
       kCols * buttonW + (kCols - 1) * buttonSpacingX + 2 * panelPaddingX,   // width
    kRows * buttonH + (kRows - 1) * buttonSpacingY + 2 * panelPaddingY   // height
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
    else if (action == "Magic")
    {
        ShowActionMessage("You have no magic yet!");
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
    if (m_messageActive)
        DismissActionMessage();

    m_messageActive = true;

    constexpr BB_FRectangle bgRect{ 60.f, 160.f, 180.f, 48.f };
    InterfaceTexture::TextureInfo bgInfo{
        .pTextureFile = "../Assets/UI/BottomTextBox.png",
        .spriteDimensions = {16, 16},
        .useFullImage = true
    };
    m_messageRoot.AddNode<InterfaceTexture>("ActionMessage_BG", bgRect, bgInfo);

    // --- Text parameters ---
    BB_FRectangle txtRect{ 68.f, 166.f, 164.f, 36.f };
    InterfaceText::Paremeters params{};
    params.pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf";
    params.textSize = 16;
    params.color = ColorPresets::white;
    params.pText = text.c_str();

    m_messageNode = m_messageRoot.AddNode<InterfaceText>("ActionMessage_Text", txtRect, params);

    // Add to screen
    m_messageRoot.AddToScreen();
}

void EncounterComponent::DismissActionMessage()
{
    if (!m_messageActive) 
        return;

    m_messageRoot.RemoveFromScreen();
    m_messageNode = nullptr;
    m_messageActive = false;
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