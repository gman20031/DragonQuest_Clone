#pragma once
#include <Actors/Component.h>
#include <../Black Box Engine/Math/FVector2.h>
class BlackBoxGame;

class TakeComponent: public BlackBoxEngine::Component
{
    GENERATE_ID("TakeComponent");


public:
    TakeComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) { /*EMPTY*/ }
    virtual ~TakeComponent() override {}

    //virtual FVector2 GetTargetPosition() const { return m_targetPosition; }

    virtual void OnTakeUsed(BlackBoxEngine::Actor* pOtherActor); // all stairs must implement this

    virtual void Start() override {} // only when this is created.
    virtual void Update() override {} // every tick of the game
    virtual void Render() override {} // just for rendering stuff
    virtual void OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override; // if this actor has a collider, and walks into another actor with a collider
    virtual void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be saved
    virtual void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be loaded
};

