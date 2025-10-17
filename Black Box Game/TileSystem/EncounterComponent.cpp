#include "EncounterComponent.h"
#include <Resources/ResourceManager.h>

#include "EncounterHandler.h"
#include "../Black Box Engine/Actors/ActorManager.h"
#include "../Black Box Engine/BlackBoxManager.h"
#include "../Black Box Engine/Actors/EngineComponents/SpriteComponent.h"
#include "../Black Box Game/InteractionComponent.h"
#include "../Black Box Game/PlayerMovementComponent.h"

using namespace BlackBoxEngine;

void EncounterComponent::Start()
{
    //BB_LOG(LogType::kMessage, "Enemy '%s' spawned with HP=%d", m_name.c_str(), m_hp);
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
    m_inBattle = false;
    BB_LOG(LogType::kMessage, "Encounter ended.");
}

float EncounterComponent::RandomFloat()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void EncounterComponent::Update()
{
   
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

    if (m_name == "BlueSlime")
    {
        BasicAttack();
    }
    else if (m_name == "RedSlime")
    {
        if (roll < 0.8f)
            BasicAttack();
        else
            Taunt(m_name);
    }
    else if (m_name == "Ghost")
    {
        if (roll < 0.5f)
            BasicAttack();
        else
            Dodge();
    }
    else if (m_name == "Drakee")
    {
        if (roll < 0.3f)
            SwoopAttack();
        else
            BasicAttack();
    }
    else if (m_name == "Magician")
    {
        //if (roll < 0.5f)
        //    CastSpell("Hurt");
        //else
        //    BasicAttack();
    }
    else
    {
        BasicAttack();
    }
}



//MAYBE ADD A COMBAT OR STATS COMP FOR THE PLAYER?
// 
//not sure those should be here -? maybe have a combat comp on player?
void EncounterComponent::PlayerAttack()
{
    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pStats) return;

    int playerAtk = pStats->GetPlayerStrength();
    int damage = std::max(1, playerAtk - m_defense);
    m_hp -= damage;

    BB_LOG(LogType::kMessage, "You hit the %s for %d damage!", m_name.c_str(), damage);

    if (m_hp <= 0)
    {
        BB_LOG(LogType::kMessage, "The %s is defeated!", m_name.c_str());
        EndCombatUI();
        EndEncounter();

        //pStats->Set(m_xpReward); //i need the XP
        pStats->SetPlayerGold(pStats->GetPlayerGold() + m_goldReward);

        //if (auto* inter = m_pPlayer->GetComponent<InteractionComponent>())
        //{
        //    inter->m_forceHUDVisible = true;  // Keep HUD visible during combat
        //    // Immediately refresh HUD to show current stats
        //    auto* stats = m_pPlayer->GetComponent<PlayerStatsComponent>();
        //    if (stats)
        //        stats->RefreshHUD();
        //}


        if (auto* playerMove = m_pPlayer->GetComponent<PlayerMovementComponent>())
        {
            playerMove->SetAnimationPaused(false);
            playerMove->m_stopMoving = false;
        }

        BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();
    }
    else
    {
        // Enemy retaliates immediately
        EnemyTakeTurn();
    }
}

void EncounterComponent::TryToFlee()
{
    if (!m_inBattle)
        return;

    float fleeChance = 0.5f;
    if (m_name == "Drakee" || m_name == "Magician")
        fleeChance = 0.35f;
    else if (m_name == "Ghost")
        fleeChance = 0.25f;

    if (RandomFloat() < fleeChance)
    {
        BB_LOG(LogType::kMessage, "You successfully escaped!");
        EndCombatUI();
        EndEncounter();
        m_inBattle = false;

        if (auto* playerMove = m_pPlayer->GetComponent<PlayerMovementComponent>())
        {
            playerMove->SetAnimationPaused(false);
            playerMove->m_stopMoving = false;
        }

        BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();
    }
    else
    {
        BB_LOG(LogType::kMessage, "You cannot escape!");
        EnemyTakeTurn(); // immediately counterattack if flee fails
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

    //if (auto* inter = m_pPlayer->GetComponent<InteractionComponent>())
    //    inter->RefreshHUD();

    BB_LOG(LogType::kMessage, "The %s attacks! You take %d damage!", m_name.c_str(), damage);

    // Check if player died
    if (pStats->GetPlayerHP() <= 0)
    {
        BB_LOG(LogType::kMessage, "You are defeated!");
        EndCombatUI();
        EndEncounter();

        if (auto* playerMove = m_pPlayer->GetComponent<PlayerMovementComponent>())
        {
            playerMove->SetAnimationPaused(false);
            playerMove->m_stopMoving = false;
        }
        BlackBoxManager::Get()->m_pInputManager->SwapInputToGame();
    }
}

void EncounterComponent::CastSpell()
{
    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pStats) return;

    //if (spellName == "Hurt")
    //{
    //    int damage = 8 + rand() % 4;
    //    pStats->TakeDamage(damage);
    //    BB_LOG(LogType::kMessage, "The %s casts Hurt! You take %d damage!", m_name.c_str(), damage);
    //}
}

void EncounterComponent::Taunt(const std::string& enemyName)
{
    BB_LOG(LogType::kMessage, "%s glares menacingly!", enemyName.c_str());
}

void EncounterComponent::Dodge()
{
    BB_LOG(LogType::kMessage, "%s fades away briefly...", m_name.c_str());
}

void EncounterComponent::SwoopAttack()
{
    auto* pStats = m_pPlayer->GetComponent<PlayerStatsComponent>();
    if (!pStats) return;

    //int damage = std::max(1, static_cast<int>(m_attack * 1.2f) - pStats->GetPlayerDefense());
    //pStats->TakeDamage(damage);

    //BB_LOG(LogType::kMessage, "The %s swoops down and hits for %d damage!", m_name.c_str(), damage);
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
    parser.GetChildVariable("SpriteFile", &m_spriteFile);
    //parser.GetChildVariable("PatrolRadius", &m_patrolRadius);
}

void EncounterComponent::Save(XMLElementParser parser)
{
    parser.NewChildVariable("Name", m_name);
    parser.NewChildVariable("HP", m_hp);
    parser.NewChildVariable("Attack", m_attack);
    parser.NewChildVariable("Defense", m_defense);
    parser.NewChildVariable("XPReward", m_xpReward);
    parser.NewChildVariable("GoldReward", m_goldReward);
    parser.NewChildVariable("SpriteFile", m_spriteFile);
    //parser.NewChildVariable("PatrolRadius", m_patrolRadius);
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
    m_pPlayer->GetComponent<InteractionComponent>()->m_uiActive = false;

    
}

void EncounterComponent::OnCombatButtonPressed(const std::string& action)
{
    if (action == "Fight")
    {
        // Call attack logic
        PlayerAttack();


    }
    else if (action == "Magic")
    {
        BB_LOG(LogType::kMessage, "You have no magic yet!");
    }
    else if (action == "Item")
    {
        BB_LOG(LogType::kMessage, "You rummage through your bag...");
    }

    //THIS TEMPS
    else if (action == "Run")
    {
        TryToFlee();

    }
}