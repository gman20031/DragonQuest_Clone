#pragma once
#include <Actors/Component.h>
#include <../Black Box Engine/Math/FVector2.h>
class BlackBoxGame;

class StairComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("StairComponent");

protected:
    //FVector2 m_targetPosition{ 0,0 };

public:
    StairComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) { /*EMPTY*/ }
    virtual ~StairComponent() override {}

    //virtual FVector2 GetTargetPosition() const { return m_targetPosition; }

    virtual void OnStairUsed(BlackBoxEngine::Actor* pOtherActor) = 0; // all stairs must implement this

    virtual void Start() override; // only when this is created.
    virtual void Update() override; // every tick of the game
    virtual void Render() override; // just for rendering stuff
    virtual void OnCollide(BlackBoxEngine::Actor* pOtherActor) override; // if this actor has a collider, and walks into another actor with a collider
    virtual void Save(BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be saved
    virtual void Load(const BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be loaded
};

class CaveEntranceComponent : public StairComponent
{
    GENERATE_ID("CaveEntranceComponent");

public:
    CaveEntranceComponent(BlackBoxEngine::Actor* pActor) : StairComponent(pActor) {}
    virtual ~CaveEntranceComponent() override {}
    
    virtual void OnStairUsed(BlackBoxEngine::Actor* pOtherActor) override;

    virtual void Start() override; // only when this is created.
    virtual void Update() override; // every tick of the game
    virtual void Render() override; // just for rendering stuff
    virtual void OnCollide(BlackBoxEngine::Actor* pOtherActor) override; // if this actor has a collider, and walks into another actor with a collider
    virtual void Save(BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be saved
    virtual void Load(const BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be loaded
};


