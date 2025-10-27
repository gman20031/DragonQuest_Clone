#pragma once
#include <Actors/Component.h>
#include <../Black Box Engine/Math/FVector2.h>
#include <string>

class BlackBoxGame;

using namespace BlackBoxEngine;

class BaseStairComponent : public Component
{
    GENERATE_ID("BaseStairComponent");

    uint64_t m_messageId = 0;

public:
    struct TransitionData {
        std::string targetLevelPath;
        FVector2 newPosition{ 0,0 };
        float fadeDuration = 1.0f;
        bool autoUse = false;
        std::string musicTrackPath;
        std::string soundTrackPath;
        float volume = 4.0f;
    };

    struct PlayerRuntimeStats {
        int HP;
        int MP;
        int Gold;
        int Energy;
    };
    
    explicit BaseStairComponent(Actor* pOwner): Component(pOwner) {}
    virtual ~BaseStairComponent();
    explicit BaseStairComponent(const TransitionData& data): Component(nullptr), m_data(data) {}

    void SetTransitionData(const TransitionData& data) { m_data = data; }
    const TransitionData& GetTransitionData() const { return m_data; }

    virtual void Start() override;

    virtual void OnCollide(Actor* pOtherActor) override;
    virtual void Save(XMLElementParser parser) override;
    virtual void Load(const XMLElementParser parser) override;
    virtual void OnStairUsed(Actor* pOtherActor);

private:
    TransitionData m_data;
};
