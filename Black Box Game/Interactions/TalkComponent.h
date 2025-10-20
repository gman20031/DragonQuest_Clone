#pragma once
#include <Actors/Component.h>
#include <../Black Box Engine/Math/FVector2.h>
class BlackBoxGame;

class BaseTalkComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("BaseTalkComponent");

public:
    struct TalkData
    {
        bool requiresItem = false;
        std::string requiredItemName;
    };

    explicit BaseTalkComponent(BlackBoxEngine::Actor* pOwner)
        : Component(pOwner) {
    }
    virtual ~BaseTalkComponent() override = default;

    void SetTalkData(const TalkData& data) { m_data = data; }
    const TalkData& GetTalkData() const { return m_data; }

    virtual void OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override;
    virtual void OnTalkUsed([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) {}
    virtual void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override;
    virtual void Load(const [[maybe_unused]]BlackBoxEngine::XMLElementParser parser) override;

protected:
    TalkData m_data;
};


class InnTalkComponent : public BaseTalkComponent
{
    GENERATE_ID("InnTalkComponent");

public:
    using BaseTalkComponent::BaseTalkComponent;

    void OnTalkUsed([[maybe_unused]]BlackBoxEngine::Actor* pOtherActor) override; 
};

class CastleTalkComponent : public BaseTalkComponent
{
    GENERATE_ID("CastleTalkComponent");

public:
    using BaseTalkComponent::BaseTalkComponent;

    void OnTalkUsed([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override;
};