#include "EncounterComponent.h"
#include <Resources/ResourceManager.h>
#include <Actors/Actor.h>
#include <BlackBoxManager.h>
#include "EncounterHandler.h"
#include "../Black Box Engine/Actors/ActorManager.h"
#include "../Black Box Engine/BlackBoxManager.h"

using namespace BlackBoxEngine;

struct EnemyData
{
    std::string name;
    int hp;
    int attack;
    int defense;
    int xp;
    int gold;
    std::string sprite;
};

static const std::unordered_map<EnemyType, EnemyData> s_enemyDatabase = {
    { EnemyType::Slime,     { "Slime",     3,  5, 0, 1, 1, "sprites/enemies/slime.png" } },
    { EnemyType::RedSlime,  { "Red Slime", 4,  7, 1, 1, 2, "sprites/enemies/red_slime.png" } },
    { EnemyType::Drakee,    { "Drakee",    6,  9, 2, 2, 2, "sprites/enemies/drakee.png" } },
    { EnemyType::Ghost,     { "Ghost",     7, 11, 3, 3, 3, "sprites/enemies/ghost.png" } },
    { EnemyType::Magician,  { "Magician", 13, 11, 4, 9, 11, "sprites/enemies/magician.png" } }
};

void EncounterComponent::Start()
{
    BB_LOG(LogType::kMessage, "Enemy '%s' spawned with HP=%d", m_name.c_str(), m_hp);
}

void EncounterComponent::StartEncounter()
{


}

void EncounterComponent::EndEncounter()
{
   
}

void EncounterComponent::Update()
{
    //i need to define there behavior
    if (!m_inBattle) return;

    if (m_name == "Slime")
    {
        // simple attack behavior
        //AttackTarget();
    }
    else if (m_name == "Magician")
    {
        // maybe 50% chance to cast "Hurt" spell
        //if (RandomFloat() < 0.5f)
        //    CastSpell("Hurt");
        //else
        //    AttackTarget();
    }
    else
    {
        // default behavior
        //AttackTarget();
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

void EncounterComponent::Init(EnemyType type)
{
    m_type = type;

    auto it = s_enemyDatabase.find(type);
    if (it != s_enemyDatabase.end())
    {
        const auto& data = it->second;
        m_name = data.name;
        m_hp = data.hp;
        m_attack = data.attack;
        m_defense = data.defense;
        m_xpReward = data.xp;
        m_goldReward = data.gold;
        m_spriteFile = data.sprite;
    }
}
