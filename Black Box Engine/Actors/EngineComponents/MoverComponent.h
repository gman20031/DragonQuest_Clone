#pragma once

#include "../Component.h"
#include "TransformComponent.h"
#include "../../Math/FVector2.h"

namespace BlackBoxEngine
{
    class MoverComponent : public Component
    {
        TransformComponent* pTransform;

    public:
        GENERATE_ID("MoverComponent");
        MoverComponent(Actor* pOwner);

        FVector2 m_velocity = { 0,0 };

        virtual void Update() override;

        virtual void Load(const XMLElementParser parser) override;
        virtual void Save(XMLElementParser parser) override;
    };

}