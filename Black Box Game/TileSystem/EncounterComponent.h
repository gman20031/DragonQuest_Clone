#pragma once

#include <Actors/Component.h>
#include <Actors/Actor.h>
#include <BlackBoxManager.h>
#include <Interface/UserInterface.h>
#include <Interface/InterfaceButton.h>
#include <Interface/InterfaceText.h>
#include <Interface/InterfaceTexture.h>
#include "../Black Box Game/PlayerStatsComponent.h"


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

class BlackBoxGame;

using namespace BlackBoxEngine;

class EncounterComponent : public BlackBoxEngine::Component
{
    GENERATE_ID( "EncounterComponent" );

    //bool m_render;

    bool m_inEncounter = false;
    BlackBoxEngine::Actor* m_currentEnemy = nullptr;

    EnemyType m_type = EnemyType::Unknown;
    BlackBoxEngine::UserInterface m_combatRoot;

    BlackBoxEngine::Actor* m_pPlayer = nullptr;
public:

    BattleState m_battleState = BattleState::WaitingForPlayer;

    void SetPlayer(BlackBoxEngine::Actor* pActor) { m_pPlayer = pActor; }

public:
    EncounterComponent( BlackBoxEngine::Actor* pOwner ) : Component( pOwner ) {};
    virtual ~EncounterComponent() = default;

    void StartEncounter(Actor* pOtherActor);
    void EndEncounter();

    void Start() override;
    void Update() override;
    void Render() override {}
    void OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override {}
    void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override;
    void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override;

    // Enemy properties
    //void Init(EnemyType type);

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
    void EndCombatUI();
    void OnCombatButtonPressed(const std::string& action);
    float RandomFloat();

    void ShowActionMessage(const std::string& text);
    void DismissActionMessage();
    void RespawnPlayer();
private:

    BlackBoxEngine::InterfaceText* m_messageNode = nullptr;
    bool m_messageActive = false;
    BlackBoxEngine::UserInterface m_messageRoot;
    //thing should be based on the different type of enemies

    //should i have also a variable for the percentage of encountering depending on are
    std::string m_name;
    int m_hp = 10;
    int m_attack = 2;
    int m_defense = 1;
    int m_xpReward = 3;
    int m_goldReward = 2;
    int m_agility = 0;

    std::string m_spriteFile;
    float m_patrolRadius = 0.0f;

    bool m_inBattle = false;
    bool m_waitingForExit = false;


};