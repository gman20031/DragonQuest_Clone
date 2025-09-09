#include "MovementBlocker.h"

#include "../Actor.h"
#include "../Collision/AABBColliderComponent.h"

namespace BlackBoxEngine
{
    MovementBlocker::MovementBlocker(Actor* pActor)
        : Component(pActor)
    {
        m_pThisActorsTransform = pActor->GetComponent<TransformComponent>();
    }

    void MovementBlocker::OnCollide(Actor * pActor)
    {
        auto* collider = pActor->GetComponent<AABBColliderComponent>();
        if (!collider->Blockable())
            return;
        TransformComponent* pOtherTransform = pActor->GetComponent<TransformComponent>();
        pOtherTransform->m_position = pOtherTransform->m_prevPosition;
    }
};

