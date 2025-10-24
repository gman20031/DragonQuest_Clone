#pragma once

#include <string>
#include <vector>
#include <Actors/Component.h>
#include <Interface/InterfaceText.h>
#include <Interface/UserInterface.h>
#include <Actors/EngineComponents/TransformComponent.h>

class BlackBoxGame;

//https://mikesrpgcenter.com/dw1/levels.html
//do i do the HUD here or keep in interaction comp
class PlayerStatsComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("PlayerStatsComponent");
    friend class InteractionComponent;
    std::vector<uint64_t> m_messageIds;

    bool m_hudVisible = false;
    bool m_changingLevel = false;
    bool m_callbackActive = false;
    uint64_t m_callbackId = 0;

    BlackBoxEngine::TransformComponent* m_pTransform = nullptr;

    BlackBoxEngine::UserInterface m_hudRoot;
    BlackBoxEngine::InterfaceText* m_hudStatsText = nullptr;

    std::vector<uint64_t> m_keyDownCodes;

public:
    //i need to check how they change depending on XP!!!
    int m_playerLevel = 1;
    int m_playerMaxHP = 16;
    int m_playerHP = 16; // current HP
    int m_playerMaxMP = 0;
    int m_playerMP = 0; // current MP
    int m_playerGold = 120;
    int m_playerEXP = 10;
    int m_playerStrength = 3;
    int m_playerAgility = 3;

public:
    PlayerStatsComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) {}
    virtual ~PlayerStatsComponent();

    void SetPlayerLevel( int value );
    void SetPlayerHP( int value );
    void SetPlayerMaxHP( int value ) { m_playerMaxHP = value; };
    void SetPlayerMP( int value );
    void SetPlayerMaxMP( int value ) { m_playerMaxMP = value; };
    void SetPlayerGold( int value );
    void SetPlayerExperience( int value );

    void SetPlayerStrength(int value) { m_playerStrength = value;}
    void SetPlayerAgility(int value) { m_playerAgility = value;}

    int GetPlayerLevel() const { return m_playerLevel; }
    int GetPlayerHP()  const { return m_playerHP; }
    int GetPlayerMP()  const { return m_playerMP; }
    int GetPlayerMaxHP()  const { return m_playerMaxHP; }
    int GetPlayerMaxMP()  const { return m_playerMaxMP; }
    int GetPlayerGold()  const { return m_playerGold; }
    int GetPlayerExperience()  const { return m_playerEXP; }

    int GetPlayerStrength() const { return m_playerStrength; }
    int GetPlayerAgility() const { return m_playerAgility; }

    void CalculatePlayerXP(); 
    virtual void Update() override;
    virtual void Start() override;
    virtual void Load( [[maybe_unused]] const BlackBoxEngine::XMLElementParser parser ) override;
    virtual void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override;
private:
    void CreateHud();
    void DisplayHUD(); 
    void HideHUD();
    void RefreshHUD();
    std::string BuildStatsString() const;

    void OnLevelChange();
};

