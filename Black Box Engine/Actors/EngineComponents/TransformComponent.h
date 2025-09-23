#pragma once

#include "../Component.h"
#include "../../Math/FVector2.h"

namespace BlackBoxEngine
{
	class TransformComponent : public Component
	{
		GENERATE_ID("TransformComponent");
	public:
        TransformComponent(Actor* pOwner) : Component(pOwner) {/*EMPTY*/ }
        virtual ~TransformComponent() = default;

        FVector2 m_prevPosition = { 0,0 };
        FVector2 m_position = { 0,0 };
        float m_rotation = 0;

        virtual void Load(const XMLElementParser parser) override;
        virtual void Save(XMLElementParser parser) override;
    };

}