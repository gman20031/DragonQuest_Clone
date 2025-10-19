#pragma once
#include <Actors/Component.h>

#include <cstdint>
#include <string>
#include <vector>
#include <array>

#include <BlackBoxManager.h>
#include <Input/InputManager.h>
#include <Math/FVector2.h>
#include <Interface/UserInterface.h>
#include <Interface/InterfaceButton.h>
#include <Interface/InterfaceText.h>
#include <Interface/InterfaceTexture.h>

#include "StairComponent.h"
#include "TalkComponent.h"
#include "TakeComponent.h"

#include "../PlayerStatsComponent.h"

class BlackBoxGame;


class InteractionComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("InteractionComponent");
    friend class PlayerStatsComponent;
    friend class EncounterComponent;

    // -- Open / Close UI keyCodes;
    uint64_t m_keyDownCodes[2];

    // --- UI Roots ---
    BlackBoxEngine::UserInterface m_pCommandMenuRootNode;
    BlackBoxEngine::UserInterface m_messageRootInterface;
    BlackBoxEngine::InterfaceTexture* m_pMessageBackgroundNode;

    // --- Message Box ---
    bool m_messageActive = false;

    // --- Actor references ---
    BlackBoxEngine::Actor* m_playerActor = nullptr;

    // --- Interaction targets ---
    BaseStairComponent* m_pCurrentStair = nullptr;
    TalkComponent* m_pCurrentTalk = nullptr;
    TakeComponent* m_pCurrentTake = nullptr;

    // --- State ---
    bool m_commandMenuActive = false;
    bool m_didMove = false;
    bool m_isChangingLevel = false;
    bool m_needsHUDRefresh = false;
    bool m_forceHUDVisible = false;

    uint64_t m_delayedDisplayId = 0;

public:
    InteractionComponent( BlackBoxEngine::Actor* pOwner );
    virtual ~InteractionComponent();

    // --- Engine Overrides ---
    void Start() override;
    void Update() override;
    void Render() override;
    void OnCollide(BlackBoxEngine::Actor* pOtherActor) override;
    void Save(BlackBoxEngine::XMLElementParser parser) override;
    void Load(const BlackBoxEngine::XMLElementParser parser) override;

    // --- Player Linking ---
    void SetPlayerActor(BlackBoxEngine::Actor* actor) { m_playerActor = actor; }

    // --- Interaction Hooks ---
    void OnButtonPressed(const std::string& action);
    void OnLevelTransitionStart();
    void OnLevelTransitionEnd();

    void SetCurrentStair(BaseStairComponent* stair) { m_pCurrentStair = stair; }
    void SetCurrentTalk(TalkComponent* talk) { m_pCurrentTalk = talk; }
    void SetCurrentTake(TakeComponent* take) { m_pCurrentTake = take; }

private:
    bool CreateCommandMenuUI();
    bool CreateMessageLogBox();

    // --- UI helpers ---
    void OpenCommandUI();
    void CloseCommandUI();

    void ShowActionMessage(const std::string& text);
    void DismissActionMessage();

    // --- Action Handlers ---
    void HandleTalk();
    void HandleStair();
    void HandleTake();

};