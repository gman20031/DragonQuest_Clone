#pragma once
#include <Actors/Component.h>

class BlackBoxGame;

enum class EnemyType
{
    Slime,
    RedSlime,
    Drakee,
    Ghost,
    Magician,
    Unknown
};

class EnemyComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("EnemyComponent");

    EnemyType m_type = EnemyType::Unknown;

public:
    EnemyComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) {}
    virtual ~EnemyComponent() = default;

    void Start() override;
    void Update() override;
    void Render() override {}
    void OnCollide([[maybe_unused]]BlackBoxEngine::Actor* pOtherActor) override {}
    void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override;
    void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override;

    // Enemy properties
    void Init(EnemyType type);

    const std::string& GetName() const { return m_name; }
    int GetHP() const { return m_hp; }
    int GetAttack() const { return m_attack; }
    int GetDefense() const { return m_defense; }
    int GetXP() const { return m_xpReward; }
    int GetGold() const { return m_goldReward; }
    const std::string& GetSpritePath() const { return m_spriteFile; }

    void SetActiveInBattle(bool active) { m_inBattle = active; }

private:

    //thing should be based on the different type of enemies

    //should i have also a variable for the percentage of encountering depending on are
    std::string m_name;
    int m_hp = 10;
    int m_attack = 2;
    int m_defense = 1;
    int m_xpReward = 3;
    int m_goldReward = 2;

    std::string m_spriteFile;
    float m_patrolRadius = 0.0f;

    bool m_inBattle = false;


};

