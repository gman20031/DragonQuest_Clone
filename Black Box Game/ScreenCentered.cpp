#include "ScreenCentered.h"

#include <System/Log.h>
#include <BlackBoxManager.h>
#include <Actors/EngineComponents/TransformComponent.h>

using namespace BlackBoxEngine;

ScreenCentered::ScreenCentered(Actor* pOwner)
    : Component(pOwner)
    , m_pWindow(BlackBoxManager::Get()->GetWindow())
{
    m_pTransform = FindOrCreateComponent<TransformComponent>(pOwner);
    m_screenDimensions = m_pWindow->GetDimensions();
    CenterActor();
}

void ScreenCentered::SetOffset(FVector2 offset)
{
    m_offset = offset;
}

void ScreenCentered::SetOffset(float xOffset, float yOffset)
{
    m_offset = { xOffset, yOffset };
}

void ScreenCentered::CenterActor() const
{
    float x = static_cast<float>(m_screenDimensions.w / 2) + m_offset.x;
    float y = static_cast<float>(m_screenDimensions.h / 2) + m_offset.y;
    m_pTransform->m_position = { x,y };
}

void ScreenCentered::Update()
{
    BB_IntRectangle currentDimensions = m_pWindow->GetDimensions();
    if (currentDimensions.w == m_screenDimensions.w && currentDimensions.h == m_screenDimensions.h)
        return;
    m_screenDimensions = currentDimensions;
    CenterActor();
}

void ScreenCentered::Save(XMLElementParser parser)
{
    parser.NewChildVariable("xOffset", m_offset.x);
    parser.NewChildVariable("yOffset", m_offset.y);
}

void ScreenCentered::Load(const XMLElementParser parser)
{
    parser.GetChildVariable("xOffset", &m_offset.x);
    parser.GetChildVariable("yOffset", &m_offset.y);
    CenterActor();
}