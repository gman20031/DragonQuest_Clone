#pragma once

#include <BlackBoxManager.h>
#include <Actors/Component.h>
#include <Actors/Actor.h>
#include <Interface/UserInterface.h>

#include <Interface/InterfaceTexture.h>
#include <Math/Random.h>

#include "../PlayerStatsComponent.h"

#include "../Black Box Game/InterfaceScrollingText.h"

using namespace BlackBoxEngine;

class EncounterComponent : public BlackBoxEngine::Component
{
    GENERATE_ID( "EncounterComponent" );

    BlackBoxEngine::Random::MachineXoshiro256 m_randomMachine;
    
    BlackBoxEngine::UserInterface m_combatRoot;
    BlackBoxEngine::ScrollingTextBox* m_pTextBox;
    BlackBoxEngine::InterfaceTexture* m_pMessageBackground = nullptr;
    BlackBoxEngine::InterfaceTexture* m_pCommandBackground = nullptr;
    BlackBoxEngine::Actor* m_pPlayer = nullptr;

    std::string m_spriteFile;
    std::string m_name;
    int m_hp = 0;
    int m_dodgeChance = 0;
    int m_attack = 0;
    int m_defense = 0;
    int m_xpReward = 0;
    int m_goldReward = 0;
    int m_agility = 0;

    bool m_playerDead = false;

    // --Item menu--
    UserInterface m_itemMenuInterface;
    InterfaceTexture* m_pItemBackgroundNode = nullptr;
    bool m_itemMenuActive = false;

    std::vector<std::string> m_itemTextStorage;
public:
    EncounterComponent( BlackBoxEngine::Actor* pOwner );
    virtual ~EncounterComponent();

    void StartEncounter(Actor* pOtherActor);
    void EndEncounter();

    virtual void Update() override;
    void Load(const BlackBoxEngine::XMLElementParser parser) override;
    void Save(BlackBoxEngine::XMLElementParser parser) override;

private:
    void PlayerDies();
    void PlayerAttack();
    void TryToFlee();

    void DoEnemyAction();
    void EnemyTakeTurn();
    void BasicAttack();
    void CastSpell(const std::string& string);

    void StartCombatUI();
    void CreateCommandButtons( InterfaceTexture* pBackground );
    void OnCombatButtonPressed(const std::string& action);
    
    void ShowActionMessage(const std::string& text , std::function<void()> onComplete = nullptr );
    void DismissActionMessage();
    void RespawnPlayer();

    bool CreateItemBox();
    void ShowItemMenu(const std::vector<std::pair<std::string, int>>& items);
    void CloseItemMenu();

   
};
 