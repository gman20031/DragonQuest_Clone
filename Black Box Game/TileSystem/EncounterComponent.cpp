#include "EncounterComponent.h"
#include <Resources/ResourceManager.h>
#include <Actors/Actor.h>
#include <BlackBoxManager.h>
#include "EncounterHandler.h"
#include "../Black Box Engine/Actors/ActorManager.h"
#include "../Black Box Engine/BlackBoxManager.h"

using namespace BlackBoxEngine;

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

