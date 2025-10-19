#pragma once

#include <BlackBoxManager.h>
#include <Actors/Component.h>
#include <Actors/Actor.h>
#include <Interface/UserInterface.h>
#include <Interface/InterfaceButton.h>
#include <Interface/InterfaceText.h>
#include <Interface/InterfaceTexture.h>
#include <Math/Random.h>

#include "../PlayerStatsComponent.h"

using namespace BlackBoxEngine;

class EncounterComponent : public BlackBoxEngine::Component
{
    GENERATE_ID( "EncounterComponent" );

    enum class EnemyType
    {
        Slime,
        RedSlime,
        Drakee,
        Ghost,
        Magician,
        Unknown
    };
    enum class BattleState
    {
        WaitingForPlayer,
        PlayerActing,
        EnemyActing,
        Victory,
        Defeat,
        Flee
    };

    EnemyType m_type = EnemyType::Unknown;
    BlackBoxEngine::Random::MachineXoshiro256 m_randomMachine;
    
    BlackBoxEngine::UserInterface m_combatRoot;
    BlackBoxEngine::InterfaceTexture* m_pMessageBackground = nullptr;
    BlackBoxEngine::Actor* m_currentEnemy = nullptr;
    BlackBoxEngine::Actor* m_pPlayer = nullptr;

    std::string m_spriteFile;
    std::string m_name;
    int m_hp = 10;
    int m_attack = 2;
    int m_defense = 1;
    int m_xpReward = 3;
    int m_goldReward = 2;
    int m_agility = 0;

    float m_patrolRadius = 0.0f;

    bool m_inBattle = false;
    bool m_inEncounter = false;
    bool m_waitingForExit = false;
    bool m_messageActive = false;
public:
    BattleState m_battleState = BattleState::WaitingForPlayer;

public:
    EncounterComponent( BlackBoxEngine::Actor* pOwner );
    virtual ~EncounterComponent() = default;

    void StartEncounter(Actor* pOtherActor);
    void EndEncounter();

    void Update() override;
    void Render() override {}
    void OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override {}
    void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override;
    void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override;

private:
    const std::string& GetName() const { return m_name; }
    int GetHP() const { return m_hp; }
    int GetAttack() const { return m_attack; }
    int GetDefense() const { return m_defense; }
    int GetXP() const { return m_xpReward; }
    int GetGold() const { return m_goldReward; }
    const std::string& GetSpritePath() const { return m_spriteFile; }

    void SetActiveInBattle(bool active) { m_inBattle = active; }
    void EnemyTakeTurn();

    void BasicAttack();
    void CastSpell(const std::string& string);

    void PlayerAttack();
    void TryToFlee();

    void StartCombatUI();
    void CreateCommandButtons( InterfaceTexture* pBackground );
    void EndCombatUI();
    void OnCombatButtonPressed(const std::string& action);
    
    void ShowActionMessage(const std::string& text);
    void DismissActionMessage();
    void RespawnPlayer();


};