#pragma once
#include <Actors/Component.h>
#include <../Black Box Engine/Math/FVector2.h>
class BlackBoxGame;

class TalkComponent: public BlackBoxEngine::Component
{
    GENERATE_ID("TalkComponent");


public:

    struct TalkData
    {
        bool requiresItem = false;
        std::string requiredItemName;
    };

    TalkComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) { /*EMPTY*/ }
    virtual ~TalkComponent() override {}

    virtual void OnTalkUsed(BlackBoxEngine::Actor* pOtherActor); // all stairs must implement this

    void SetTalkData(const TalkData& data) { m_data = data; }
    const TalkData& GetTalkData() const { return m_data; }

    virtual void OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override; // if this actor has a collider, and walks into another actor with a collider
    virtual void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be saved
    virtual void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override {} // for when this actor is called to be loaded

protected:
    TalkData m_data;
};

