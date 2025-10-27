#pragma once
#include <Actors/Component.h>
#include <../Black Box Engine/Math/FVector2.h>
class BlackBoxGame;

class BaseTalkComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("BaseTalkComponent");

    uint64_t m_messageId = 0;
public:
    struct TalkData
    {
        bool requiresItem = false;
        std::string requiredItemName;
    };

    explicit BaseTalkComponent(BlackBoxEngine::Actor* pOwner)
        : Component(pOwner) {}

    virtual ~BaseTalkComponent();

    void SetTalkData(const TalkData& data) { m_data = data; }
    const TalkData& GetTalkData() const { return m_data; }

    virtual void OnCollide(BlackBoxEngine::Actor* pOtherActor) override;
    virtual void OnTalkUsed( [[maybe_unused]]BlackBoxEngine::Actor* pOtherActor) {}
    virtual void Save(BlackBoxEngine::XMLElementParser parser) override;
    virtual void Load(const BlackBoxEngine::XMLElementParser parser) override;

    virtual void Start() override;

    virtual void SetHasTablet( [[maybe_unused]] bool value) {}
    virtual bool GetValue() { return false; }
protected:
    TalkData m_data;
};


class InnTalkComponent : public BaseTalkComponent
{
    GENERATE_ID("InnTalkComponent");

public:
    using BaseTalkComponent::BaseTalkComponent;

    void OnTalkUsed(BlackBoxEngine::Actor* pOtherActor) override; 
};

class CastleTalkComponent : public BaseTalkComponent
{
    GENERATE_ID("CastleTalkComponent");

    bool m_hasTablet = false;

public:
    using BaseTalkComponent::BaseTalkComponent;

    void OnTalkUsed(BlackBoxEngine::Actor* pOtherActor) override;

    void SetHasTablet(bool value) override { m_hasTablet = value; }
    bool GetValue() override { return m_hasTablet; }
};