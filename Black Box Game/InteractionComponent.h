#pragma once
#include <Actors/Component.h>
#include "../Black Box Engine/BlackBoxManager.h"
#include "../Black Box Engine/Input/InputManager.h"
#include <Interface/InterfaceButton.h>
#include <Interface/InterfaceText.h>
#include <Interface/InterfaceTexture.h>
#include <Interface/UserInterface.h>
#include "StairComponent.h"
#include "TalkComponent.h"
#include "TakeComponent.h"
#include <../Black Box Engine/Math/FVector2.h>

class BlackBoxGame;
//class Actor;

class InteractionComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("InteractionComponent");

    int m_bar = 25;
    const char* m_baz = "Baz";
    std::string m_foo = "foo";

    BlackBoxEngine::UserInterface m_interfaceRoot;

    BlackBoxEngine::UserInterface m_messageRoot;   // same type as m_interfaceRoot
    BlackBoxEngine::InterfaceText* m_messageNode = nullptr;
    bool m_messageActive = false;

    BlackBoxEngine::Actor* m_currentActor = nullptr;

    StairComponent* m_pCurrentStair = nullptr;
    TalkComponent* m_pCurrentTalk = nullptr;
    TakeComponent* m_pCurrentTake = nullptr;

    BlackBoxEngine::Actor* m_playerActor = nullptr;

    bool m_didMove = false;

    //maybe i hsould not do everything in the same class
    BlackBoxEngine::UserInterface m_hudRoot;
    BlackBoxEngine::InterfaceText* m_hudTextNode = nullptr;
    bool m_hudVisible = false;
   // BB_FRectangle m_hudRect;



    int m_playerLevel = 1;
    int m_playerHP = 20;
    int m_playerMaxHP = 20;
    int m_playerMP = 8;
    int m_playerMaxMP = 8;
    int m_playerGold = 120;
    int m_playerEnergy = 10;
public:
    InteractionComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) { /*EMPTY*/ }
    virtual ~InteractionComponent();

    virtual void Start() override; // only when this is created.
    virtual void Update() override; // every tick of the game
    virtual void Render() override; // just for rendering stuff
    virtual void OnCollide(BlackBoxEngine::Actor* pOtherActor) override; // if this actor has a collider, and walks into another actor with a collider
    virtual void Save(BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be saved
    virtual void Load(const BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be loaded

    void SetCurrentActor(BlackBoxEngine::Actor* actor) { m_currentActor = actor; }
    BlackBoxEngine::Actor* GetCurrentActor() const { return m_currentActor; }

    void OnButtonPressed(const std::string& action);  

    void SetCurrentStair(StairComponent* stair) { m_pCurrentStair = stair; }
    void SetCurrentTalk(TalkComponent* talk) { m_pCurrentTalk = talk; }
    void SetCurrentTake(TakeComponent* take) { m_pCurrentTake = take; }

    bool GetDidMove() { return m_didMove; }

    BlackBoxEngine::Actor* SetPlayerActor(BlackBoxEngine::Actor* pOtherActor) { return m_playerActor = pOtherActor; }

    void DisplayHUD(); //should we make it so that we can take in the name of the player?
    
    void HideHUD();
private:

    bool m_uiActive = false;

    void TestInterfaceStuff();
    void OpenUI();
    void CloseUI();

    void ShowActionMessage(const std::string& text);

    void DismissActionMessage(); 

    std::vector<uint64_t> m_keyDownCodes;
   
};
