#pragma once

#include "../Component.h"
#include "TransformComponent.h"
#include "../../Math/FVector2.h"

namespace BlackBoxEngine
{
    class MoverComponent : public Component
    {
        GENERATE_ID("MoverComponent");
    private:
        TransformComponent* pTransform;

        FVector2 m_velocity = {0,0};
        bool m_active = true;

    public:
        MoverComponent(Actor* pOwner);
        virtual ~MoverComponent() = default;

        void StopVelocity() { m_velocity = {0,0}; }

        void SetVelocity( float x, float y );
        void SetVelocity( FVector2 velocity );
        FVector2 GetVelocity() const { return m_velocity; }

        void Disable() { m_active = false; };
        void Enable() { m_active = true; };

        virtual void Update() override;

        virtual void Load(const XMLElementParser parser) override;
        virtual void Save(XMLElementParser parser) override;
    };

}