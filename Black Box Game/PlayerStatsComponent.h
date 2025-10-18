#pragma once
#include <Actors/Component.h>
#include <functional>
#include <string>
#include <vector>
#include "../Black Box Engine/BlackBoxManager.h"
#include "../Black Box Engine/Input/InputManager.h"
#include "../Black Box Engine/Math/FVector2.h"
#include <Interface/UserInterface.h>
#include <Interface/InterfaceButton.h>
#include <Interface/InterfaceText.h>
#include <Interface/InterfaceTexture.h>

class BlackBoxGame;

//https://mikesrpgcenter.com/dw1/levels.html
//do i do the HUD here or keep in interaction comp
class PlayerStatsComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("PlayerStatsComponent");

    bool m_hudVisible = false;
    bool m_needsHUDRefresh = false;
    bool m_forceHUDVisible = false;

    BlackBoxEngine::UserInterface m_hudRoot;
    BlackBoxEngine::InterfaceText* m_hudStatsText = nullptr;

public:
    //i need to check how they change depending on XP!!!
    int m_playerLevel = 1;
    int m_playerMaxHP = 16;
    int m_playerHP = 16; // current HP
    int m_playerMaxMP = 0;
    int m_playerMP = 0; // current MP
    int m_playerGold = 120;
    int m_playerEnergy = 10;
    int m_playerStrength = 3;
    int m_playerAgility = 3;

    void Initialize() {
        m_playerHP = m_playerMaxHP;  // start full HP
        m_playerMP = m_playerMaxMP;  // start full MP
    }

    //spells?
    //strengh and agility 
    //ned xp
public:
    PlayerStatsComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) {}
    virtual ~PlayerStatsComponent() {}

    //std::function<void()> OnStatsChanged = []() {};

    void SetPlayerLevel(int value) { m_playerLevel = value;
    }
    void SetPlayerHP(int value) { m_playerHP = value; 
    }
    void SetPlayerMP(int value) { m_playerMP = value;
    }
    void SetPlayerGold(int value) { m_playerGold = value; 
    }
    void SetPlayerEnergy(int value) { m_playerEnergy = value;
    }

    void SetPlayerStrength(int value) { m_playerStrength = value;}
    void SetPlayerAgility(int value) { m_playerAgility = value;}

    int GetPlayerLevel() { return m_playerLevel; }
    int GetPlayerHP() { return m_playerHP; }
    int GetPlayerMP() { return m_playerMP; }
    int GetPlayerGold() { return m_playerGold; }
    int GetPlayerEnergy() { return m_playerEnergy; }

    int GetPlayerStrength() { return m_playerStrength; }
    int GetPlayerAgility() { return m_playerAgility; }

    void Start() override;
    void Update() override;
    void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override;
    void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override;
    //getter and setters ->set the HUD in interact comp every update
    //or do the UI for the HUD here (but needs to figure out how to make is work like u  did in the other comp)

    void DisplayHUD(); 
    void HideHUD();
    void RefreshHUD();
    std::string BuildStatsString() const;
};

