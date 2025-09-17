#include "PlayerCameraComponent.h"

#include "../../BlackBoxManager.h"
#include "../../Graphics/Camera.h"
#include "../Actor.h"
#include "TransformComponent.h"


namespace BlackBoxEngine
{
    PlayerCameraComponent::PlayerCameraComponent(Actor* pActor)
        : Component(pActor)
    {
        /*EMPTY*/
    }

    void PlayerCameraComponent::Update()
    {
        if (!m_pTransform || !m_pGameCamera)
            return;

        m_pGameCamera->SetCenterPoint(m_pTransform->m_position);
    }

    void PlayerCameraComponent::Start()
    {
        m_pTransform = FindOrCreateComponent<TransformComponent>(m_pOwner);
        m_pGameCamera = BlackBoxManager::Get()->m_pMainCamera;

        if (!m_pTransform)
            BB_LOG(LogType::kError, "Player BB_Camera component can not find a transform component");
        if (!m_pGameCamera)
            BB_LOG(LogType::kError, "Player BB_Camera component can not find a camera attached to the engine");
    }
}
