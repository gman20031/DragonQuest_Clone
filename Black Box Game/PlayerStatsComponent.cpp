#include "PlayerStatsComponent.h"

void PlayerStatsComponent::Load(const BlackBoxEngine::XMLElementParser parser)
{
    parser.GetChildVariable("Level", &m_playerLevel);
    parser.GetChildVariable("HP", &m_playerHP);
    parser.GetChildVariable("MaxHP", &m_playerMaxHP);
    parser.GetChildVariable("MP", &m_playerMP);
    parser.GetChildVariable("MaxMP", &m_playerMaxMP);
    parser.GetChildVariable("Gold", &m_playerGold);
    parser.GetChildVariable("Energy", &m_playerEnergy);
    parser.GetChildVariable("Strength", &m_playerStrength);
    parser.GetChildVariable("Agility", &m_playerAgility);
}

void PlayerStatsComponent::Save(BlackBoxEngine::XMLElementParser parser)
{
    parser.NewChildVariable("Level", &m_playerLevel);
    parser.NewChildVariable("HP", &m_playerHP);
    parser.NewChildVariable("MaxHP", &m_playerMaxHP);
    parser.NewChildVariable("MP", &m_playerMP);
    parser.NewChildVariable("MaxMP", &m_playerMaxMP);
    parser.NewChildVariable("Gold", &m_playerGold);
    parser.NewChildVariable("Energy", &m_playerEnergy);
    parser.NewChildVariable("Strength", &m_playerStrength);
    parser.NewChildVariable("Agility", &m_playerAgility);
}
