#pragma once
#include <Actors/Component.h>
#include <../Black Box Engine/Math/FVector2.h>
class BlackBoxGame;

class StairComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("StairComponent");

protected:

public:
    StairComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) { /*EMPTY*/ }
    virtual ~StairComponent() override {}

    //virtual FVector2 GetTargetPosition() const { return m_targetPosition; }

    virtual void OnStairUsed(BlackBoxEngine::Actor* pOtherActor) = 0; // all stairs must implement this

    virtual void Start() override {} // only when this is created.
    virtual void Update() override {} // every tick of the game
    virtual void Render() override {} // just for rendering stuff
    virtual void OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override {} // if this actor has a collider, and walks into another actor with a collider
    virtual void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be saved
    virtual void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be loaded
};

//load level 1 (cave part 1)
class CaveEntranceComponent : public StairComponent
{
    GENERATE_ID("CaveEntranceComponent");

public:
    CaveEntranceComponent(BlackBoxEngine::Actor* pActor) : StairComponent(pActor) {}
    virtual ~CaveEntranceComponent() override {}
    
    virtual void OnStairUsed([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override;

    void OpenLevel([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor);

    virtual void Start() override {} // only when this is created.
    virtual void Update() override {} // every tick of the game
    virtual void Render() override {} // just for rendering stuff
    virtual void OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override; // if this actor has a collider, and walks into another actor with a collider
    virtual void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be saved
    virtual void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be loaded
};

//load level 2 (cave part 2)
class StairDownComponent : public StairComponent
{
    GENERATE_ID("StairDownComponent");

public:
    StairDownComponent(BlackBoxEngine::Actor* pActor) : StairComponent(pActor) {}
    virtual ~StairDownComponent() override {}

    virtual void OnStairUsed([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override;

    virtual void Start() override {} // only when this is created.
    virtual void Update() override {}// every tick of the game
    virtual void Render() override {} // just for rendering stuff
    virtual void OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override;  // if this actor has a collider, and walks into another actor with a collider
    virtual void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override {}// for when this actor is called to be saved
    virtual void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be loaded
};

//load main map
class StairUpLevel1Component : public StairComponent
{
    GENERATE_ID("StairUpLevel1Component");

public:
    StairUpLevel1Component(BlackBoxEngine::Actor* pActor) : StairComponent(pActor) {}
    virtual ~StairUpLevel1Component() override {}

    virtual void OnStairUsed([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override;

    virtual void Start() override {} // only when this is created.
    virtual void Update() override {} // every tick of the game
    virtual void Render() override {} // just for rendering stuff
    virtual void OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override; // if this actor has a collider, and walks into another actor with a collider
    virtual void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be saved
    virtual void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be loaded
};

//load level 1 from level 2 
class StairUpLevel2Component : public StairComponent
{
    GENERATE_ID("StairUpLevel2Component");

public:
    StairUpLevel2Component(BlackBoxEngine::Actor* pActor) : StairComponent(pActor) {}
    virtual ~StairUpLevel2Component() override {}

    virtual void OnStairUsed([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override; 

    virtual void Start() override {} // only when this is created.
    virtual void Update() override {} // every tick of the game
    virtual void Render() override {} // just for rendering stuff
    virtual void OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override;  // if this actor has a collider, and walks into another actor with a collider
    virtual void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be saved
    virtual void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be loaded
};