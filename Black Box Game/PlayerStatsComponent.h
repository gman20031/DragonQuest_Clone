#pragma once
#include <Actors/Component.h>
class BlackBoxGame;

//https://mikesrpgcenter.com/dw1/levels.html
//do i do the HUD here or keep in interaction comp
class PlayerStatsComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("PlayerStatsComponent");

    //THIS NEEDS TO BE SET UP ACCORDINGLY 
    int m_playerLevel = 1;
    int m_playerHP = 20;
    int m_playerMaxHP = 20;
    int m_playerMP = 8;
    int m_playerMaxMP = 8;
    int m_playerGold = 120;
    int m_playerEnergy = 10;

public:
    PlayerStatsComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) {}
    virtual ~PlayerStatsComponent() {}

    //getter and setters ->set the HUD in interact comp every update
    //or do the UI for the HUD here (but needs to figure out how to make is work like u  did in the other comp)

};

