#pragma once
#include <Actors/Component.h>
#include <../Black Box Engine/Math/FVector2.h>
class BlackBoxGame;

class TakeComponent: public BlackBoxEngine::Component
{
    GENERATE_ID("TakeComponent");

    bool m_hasTablet = false;
    uint64_t m_messageId = 0;

public:
    TakeComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) { /*EMPTY*/ }
    virtual ~TakeComponent();

    virtual void OnTakeUsed(BlackBoxEngine::Actor* pOtherActor); // all stairs must implement this
    virtual void OnCollide(BlackBoxEngine::Actor* pOtherActor) override; // if this actor has a collider, and walks into another actor with a collider
    virtual void Start() override;
};

