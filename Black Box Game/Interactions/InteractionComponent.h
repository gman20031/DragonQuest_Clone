#pragma once
#include <Actors/Component.h>

#include <string>
#include <vector>

#include <Input/InputManager.h>
#include <Interface/UserInterface.h>
#include <Interface/InterfaceTexture.h>

#include "StairComponent.h"
#include "TalkComponent.h"
#include "TakeComponent.h"

#include "../PlayerStatsComponent.h"
#include "../Black Box Game/InterfaceScrollingText.h"

class BlackBoxGame;

class InteractionComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("InteractionComponent");

    // -- Open / Close UI keyCodes;
    uint64_t m_keyDownCodes[2];
    std::vector<uint64_t> m_messageIds;

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

    BaseTalkComponent* m_pCurrentTalk = nullptr;
    TakeComponent* m_pCurrentTake = nullptr;

    // --- State ---
    bool m_commandMenuActive = false;

    uint64_t m_delayedDisplayId = 0;

    // --Item menu--
    UserInterface m_itemMenuInterface;
    InterfaceTexture* m_pItemBackgroundNode = nullptr;
    bool m_itemMenuActive = false;
    std::vector<std::string> m_itemTextStorage;
    bool m_messageFromItemMenu = false;

    bool m_choiceMenuActive = false;
    std::function<void(const std::string&)> m_choiceCallback;

    ScrollingTextBox* m_activeScrollBox = nullptr;

public:
    InteractionComponent( BlackBoxEngine::Actor* pOwner );
    virtual ~InteractionComponent();

    // --- Engine Overrides ---
    void Start() override;
    void Update() override;

    // --- Player Linking ---
    void SetPlayerActor(BlackBoxEngine::Actor* actor) { m_playerActor = actor; }

    // --- Interaction Hooks ---
    void OnButtonPressed(const std::string& action);

    void SetCurrentStair(BaseStairComponent* stair) { m_pCurrentStair = stair; }
    void SetCurrentTalk(BaseTalkComponent* talk) { m_pCurrentTalk = talk; }
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
    void HandleItem();

    void OnLevelChanging();
    bool CreateItemBox(); 
    void ShowItemMenu(const std::vector<std::pair<std::string, int>>& items); 
    void CloseItemMenu();

    void DismissChoiceMenu();
    void ShowChoiceMenu(const std::vector<std::pair<std::string, int>>& choices,
        std::function<void(const std::string&)> callback);
    void SleepAtInn();
};