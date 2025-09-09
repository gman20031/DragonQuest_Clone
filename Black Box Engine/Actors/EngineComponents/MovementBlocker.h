#pragma once

#include "transformComponent.h"
#include "../Component.h"

namespace BlackBoxEngine
{
    class MovementBlocker : public Component
    {
        GENERATE_ID("MovementBlocker");
        TransformComponent* m_pThisActorsTransform;
    public:
        MovementBlocker(Actor* pActor);

        virtual void OnCollide(Actor* pActor) override;
    };


}