#pragma once
#include <Actors/Component.h>
#include <Graphics/Window.h>
#include <Graphics/RenderingStructs.h>
#include <Actors/EngineComponents/TransformComponent.h>
#include <Math/FVector2.h>

class ScreenCentered : public BlackBoxEngine::Component
{
    GENERATE_ID("ScreenCentered");
    
    const std::unique_ptr<BlackBoxEngine::BB_Window>& m_pWindow;
    BlackBoxEngine::TransformComponent* m_pTransform = nullptr;
    BlackBoxEngine::BB_IntRectangle m_screenDimensions = {};
    BlackBoxEngine::FVector2 m_offset{ 0,0 };
private:
    void CenterActor() const;
public:
    ScreenCentered(BlackBoxEngine::Actor* pOwner);
    virtual ~ScreenCentered() = default;

    void SetOffset(BlackBoxEngine::FVector2 offset);
    void SetOffset(float xOffset, float yOffset);
    BlackBoxEngine::FVector2 GetOffset() const { return m_offset; }

    virtual void Update() override;
    virtual void Save(BlackBoxEngine::XMLElementParser parser) override;
    virtual void Load(const BlackBoxEngine::XMLElementParser parser) override;
};

