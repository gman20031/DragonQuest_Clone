#pragma once
#include <Actors/Component.h>
#include "../Black Box Engine/BlackBoxManager.h"
#include "../Black Box Engine/Input/InputManager.h"
#include <Interface/InterfaceButton.h>
#include <Interface/InterfaceText.h>
#include <Interface/InterfaceTexture.h>
#include <Interface/UserInterface.h>
#include "StairComponent.h"
#include <../Black Box Engine/Math/FVector2.h>
//#include "../Black Box Engine/Actors/Actor.h"

class BlackBoxGame;
//class Actor;

class InteractionComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("InteractionComponent");

    int m_bar = 25;
    const char* m_baz = "Baz";
    std::string m_foo = "foo";

    BlackBoxEngine::UserInterface m_interfaceRoot;

    BlackBoxEngine::Actor* m_currentActor = nullptr;

    StairComponent* m_currentStair = nullptr;

    BlackBoxEngine::Actor* m_playerActor = nullptr;

    bool m_didMove = false;

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

    void SetCurrentStair(StairComponent* stair) { m_currentStair = stair; }
    bool GetDidMove() { return m_didMove; }

    BlackBoxEngine::Actor* SetPlayerActor(BlackBoxEngine::Actor* pOtherActor) { return m_playerActor = pOtherActor; }
private:

    bool m_uiActive = false;

    void TestInterfaceStuff();
    void OpenUI();
    void CloseUI();




    std::vector<uint64_t> m_keyDownCodes;
   
};
