#include "MoverComponent.h"

#include "../../BlackBoxManager.h"
#include "../../Resources/XML/BlackBoxXMLParser.h"

BlackBoxEngine::MoverComponent::MoverComponent(Actor* pOwner)
    : Component(pOwner)
{
    pTransform = pOwner->GetComponent<TransformComponent>();
}

void BlackBoxEngine::MoverComponent::SetVelocity( float x, float y )
{
    SetVelocity( {x,y} );
}

void BlackBoxEngine::MoverComponent::SetVelocity( FVector2 velocity )
{
    m_velocity = velocity;
}

void BlackBoxEngine::MoverComponent::Update()
{
    double deltaTime = BlackBoxManager::Get()->GetDeltaTime();
    pTransform->m_prevPosition = pTransform->m_position;
    pTransform->m_position += m_velocity * (float)deltaTime;
}

void BlackBoxEngine::MoverComponent::Load(const XMLElementParser parser)
{  
    auto pVelocity = parser.GetChildElement("Velocity");

    pVelocity.GetChildVariable("X", &m_velocity.x);
    pVelocity.GetChildVariable("Y", &m_velocity.y);
}

void BlackBoxEngine::MoverComponent::Save(XMLElementParser parser)
{
    auto pVelocity = parser.InsertNewChild("Velocity");

    pVelocity.NewChildVariable("X", m_velocity.x);
    pVelocity.NewChildVariable("Y", m_velocity.y);
}
