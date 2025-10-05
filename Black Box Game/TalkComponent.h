#pragma once
#include <Actors/Component.h>
#include <../Black Box Engine/Math/FVector2.h>
class BlackBoxGame;

class TalkComponent: public BlackBoxEngine::Component
{
    GENERATE_ID("TalkComponent");


public:
    TalkComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) { /*EMPTY*/ }
    virtual ~TalkComponent() override {}

    //virtual FVector2 GetTargetPosition() const { return m_targetPosition; }

    virtual void OnTalkUsed(BlackBoxEngine::Actor* pOtherActor); // all stairs must implement this

    virtual void Start() override {} // only when this is created.
    virtual void Update() override {} // every tick of the game
    virtual void Render() override {} // just for rendering stuff
    virtual void OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override; // if this actor has a collider, and walks into another actor with a collider
    virtual void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be saved
    virtual void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be loaded
};

