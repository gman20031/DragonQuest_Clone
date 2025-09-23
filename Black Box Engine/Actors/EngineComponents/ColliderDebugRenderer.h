#pragma once

#include "../Component.h"

#include "../../Graphics/Color.h"

namespace BlackBoxEngine
{
    class BB_Renderer;
    class TransformComponent;
    class AABBColliderComponent;

    class ColliderDebugRenderer : public Component
    {
        GENERATE_ID("ColliderDebugRenderer");

        ColorValue m_renderColor = ColorPresets::red;
        BB_Renderer* m_pRenderer = nullptr;
        TransformComponent* m_pTransform = nullptr;
        AABBColliderComponent* m_pCollider = nullptr;

    public:
        ColliderDebugRenderer(Actor* pActor);
        virtual ~ColliderDebugRenderer() = default;

        void SetRenderColor(const ColorValue& color) { m_renderColor = color; }
        const ColorValue& getRenderColor() const { return m_renderColor; }

        virtual void Render() override;

        virtual void Load(const XMLElementParser parser) override;
        virtual void Save(XMLElementParser parser) override;
    };

}
