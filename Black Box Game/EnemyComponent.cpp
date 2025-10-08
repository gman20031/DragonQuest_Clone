#include "EnemyComponent.h"
#include "../Black Box Engine/Actors/ActorManager.h"
#include "../Black Box Engine/BlackBoxManager.h"
#include "BlackBoxGame.h"

using namespace BlackBoxEngine;

void EnemyComponent::Start()
{
    BB_LOG(LogType::kMessage, "Enemy '%s' spawned with HP=%d", m_name.c_str(), m_hp);
}

void EnemyComponent::Update()
{
    // behavior

}

//to change depending on the enemy
void EnemyComponent::Load(const XMLElementParser parser)
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

void EnemyComponent::Save(XMLElementParser parser)
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