#pragma once
#include <Actors/Component.h>
class BlackBoxGame;

//https://mikesrpgcenter.com/dw1/levels.html
//do i do the HUD here or keep in interaction comp
class PlayerStatsComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("PlayerStatsComponent");

    //THIS NEEDS TO BE SET UP ACCORDINGLY 
    int m_playerLevel = 0;
    int m_playerHP = 0;
    int m_playerMaxHP = 0;
    int m_playerMP = 0;
    int m_playerMaxMP = 0;
    int m_playerGold = 0;
    int m_playerEnergy = 0;

    int m_playerStrength = 0;
    int m_playerAgility = 0;

    //spells?
    //strengh and agility 

public:
    PlayerStatsComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) {}
    virtual ~PlayerStatsComponent() {}

    void SetPlayerLevel(int value) { m_playerLevel = value; }
    void SetPlayerHP(int value) { m_playerHP = value; }
    void SetPlayerMP(int value) { m_playerMP = value; }
    void SetPlayerGold(int value) { m_playerGold = value; }
    void SetPlayerEnergy(int value) { m_playerEnergy = value; }

    void SetPlayerStrength(int value) { m_playerStrength = value; }
    void SetPlayerAgility(int value) { m_playerAgility = value; }

    int GetPlayerLevel() { return m_playerLevel; }
    int GetPlayerHP() { return m_playerHP; }
    int GetPlayerMP() { return m_playerMP; }
    int GetPlayerGold() { return m_playerGold; }
    int GetPlayerEnergy() { return m_playerEnergy; }

    int GetPlayerStrength() { return m_playerStrength; }
    int GetPlayerAgility() { return m_playerAgility; }


    void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override;
    void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override;
    //getter and setters ->set the HUD in interact comp every update
    //or do the UI for the HUD here (but needs to figure out how to make is work like u  did in the other comp)

};

