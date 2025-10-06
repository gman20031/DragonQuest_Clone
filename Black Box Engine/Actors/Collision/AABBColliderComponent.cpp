#include "AABBColliderComponent.h"

#include "CollisionManager.h"
#include "../../BlackBoxManager.h"
#include "../../System/Log.h"

namespace BlackBoxEngine
{
    AABBColliderComponent::AABBColliderComponent(Actor* pActor)
        : Component(pActor)
        , m_actorPosition(pActor->GetComponent<TransformComponent>()->m_position)
    {
    }

    AABBColliderComponent::~AABBColliderComponent()
    {
        m_pCollisionManager->RemoveActor(m_pOwner);
    }

    const BoundingBox AABBColliderComponent::GetCollider() const
    {
        return BoundingBox(m_actorPosition.x, m_actorPosition.y, m_width, m_height);
    }

    void AABBColliderComponent::DisableCollision()
    {
        if (m_enabled)
            m_pCollisionManager->RemoveActor(m_pOwner);
        m_enabled = false;
    }

    void AABBColliderComponent::EnableCollision()
    {
        if (!m_enabled)
            m_pCollisionManager->RegisterActor(m_pOwner);
        m_enabled = true;
    }

    void AABBColliderComponent::ToggleCollision()
    {
        if (m_enabled)
            DisableCollision();
        else
            EnableCollision();
    }

    void AABBColliderComponent::Update()
    {
        //BB_LOG(LogType::kMessage, "AABBColliderComponent: last position: ", m_lastPosition, " -> new position: ", newPos);
        if (m_actorPosition != m_lastPosition && m_enabled )
        {
            m_pCollisionManager->NotifyActorMoved(m_pOwner);
            m_lastPosition = m_actorPosition;
        }
    }

    void AABBColliderComponent::Start()
    {
        m_pCollisionManager = BlackBoxManager::Get()->m_pCollisionManager;
        m_pCollisionManager->RegisterActor( m_pOwner );
    }

    void AABBColliderComponent::Load(const XMLElementParser parser)
    {
        parser.GetChildVariable("width",  &m_width);
        parser.GetChildVariable("height", &m_height);
        parser.GetChildVariable("blockable", &m_IsBlockable);
        parser.GetChildVariable("enabled", &m_enabled);
    }

    void AABBColliderComponent::Save(XMLElementParser parser)
    {
        parser.NewChildVariable("width", m_width);
        parser.NewChildVariable("height", m_height);
        parser.NewChildVariable("blockable", m_IsBlockable);
        parser.NewChildVariable("enabled", m_enabled);
    }
}
