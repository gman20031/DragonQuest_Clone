#pragma once
#include <Actors/Component.h>

#include <cstdint>
#include <string>
#include <vector>
#include <array>

#include "../Black Box Engine/BlackBoxManager.h"
#include "../Black Box Engine/Input/InputManager.h"
#include "../Black Box Engine/Math/FVector2.h"
#include <Interface/UserInterface.h>
#include <Interface/InterfaceButton.h>
#include <Interface/InterfaceText.h>
#include <Interface/InterfaceTexture.h>
#include "StairComponent.h"
#include "TalkComponent.h"
#include "TakeComponent.h"

class BlackBoxGame;

class InteractionComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("InteractionComponent");

    // --- UI Roots ---
    BlackBoxEngine::UserInterface m_interfaceRoot;
    BlackBoxEngine::UserInterface m_messageRoot;
    BlackBoxEngine::UserInterface m_hudRoot;

    // --- Message Box ---
    BlackBoxEngine::InterfaceText* m_messageNode = nullptr;
    bool m_messageActive = false;

    // --- HUD ---
    bool m_hudVisible = false;

    // --- Actor references ---
    BlackBoxEngine::Actor* m_currentActor = nullptr;
    BlackBoxEngine::Actor* m_playerActor = nullptr;

    // --- Interaction targets ---
    BaseStairComponent* m_pCurrentStair = nullptr;
    TalkComponent* m_pCurrentTalk = nullptr;
    TakeComponent* m_pCurrentTake = nullptr;

    // --- State ---
    bool m_uiActive = false;
    bool m_didMove = false;
    bool m_isChangingLevel = false;

    uint64_t m_delayedDisplayId = 0;
    std::vector<uint64_t> m_keyDownCodes;

    // --- Player Stats ---
    int m_playerLevel = 1;
    int m_playerHP = 20;
    int m_playerMaxHP = 20;
    int m_playerMP = 8;
    int m_playerMaxMP = 8;
    int m_playerGold = 120;
    int m_playerEnergy = 10;

public:
    InteractionComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) {}
    virtual ~InteractionComponent();

    // --- Engine Overrides ---
    void Start() override;
    void Update() override;
    void Render() override;
    void OnCollide(BlackBoxEngine::Actor* pOtherActor) override;
    void Save(BlackBoxEngine::XMLElementParser parser) override;
    void Load(const BlackBoxEngine::XMLElementParser parser) override;

    // --- Player Linking ---
    void SetCurrentActor(BlackBoxEngine::Actor* actor) { m_currentActor = actor; }
    void SetPlayerActor(BlackBoxEngine::Actor* actor) { m_playerActor = actor; }

    // --- Interaction Hooks ---
    void OnButtonPressed(const std::string& action);
    void OnLevelTransitionStart();
    void OnLevelTransitionEnd();

    void SetCurrentStair(BaseStairComponent* stair) { m_pCurrentStair = stair; }
    void SetCurrentTalk(TalkComponent* talk) { m_pCurrentTalk = talk; }
    void SetCurrentTake(TakeComponent* take) { m_pCurrentTake = take; }

    void HideHUD();

private:
    // --- UI helpers ---
    void OpenUI();
    void CloseUI();
    void SelectionMenu();

    void DisplayHUD();

    void ShowActionMessage(const std::string& text);
    void DismissActionMessage();

    // --- Action Handlers ---
    void HandleTalk();
    void HandleStair();
    void HandleTake();

};