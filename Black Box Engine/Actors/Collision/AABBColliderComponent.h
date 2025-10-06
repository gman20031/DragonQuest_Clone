#pragma once

#include "../EngineComponents/TransformComponent.h"
#include "../Component.h"
#include "BoundingBox.h"

namespace BlackBoxEngine
{
    class CollisionManager;

    class AABBColliderComponent : public Component
    {
        GENERATE_ID("AABBColliderComponent");
        
        float m_width = 0;
        float m_height = 0;
        FVector2& m_actorPosition;
        FVector2 m_lastPosition{ 0,0 };
        bool m_IsBlockable = false;
        bool m_enabled = true;
        CollisionManager* m_pCollisionManager;

    public:
        AABBColliderComponent(Actor* pActor);
        ~AABBColliderComponent();

        const BoundingBox GetCollider() const;
        void SetWidthHeight(float w, float h) { m_width = w; m_height = h; }
        bool Blockable() const { return m_IsBlockable; }

        void DisableCollision();
        void EnableCollision();
        void ToggleCollision();

        virtual void Update() override;
        virtual void Start() override;
        virtual void Load(const XMLElementParser parser) override;
        virtual void Save(XMLElementParser parser) override;

    };


}