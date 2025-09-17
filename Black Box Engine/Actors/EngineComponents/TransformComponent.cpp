#include "TransformComponent.h"

#include "../../System/Log.h"
#include "../../BlackBoxManager.h"

namespace BlackBoxEngine
{
    void TransformComponent::Load(const XMLElementParser parser)
    {
        auto pPosition = parser.GetChildElement("Position");
        if(pPosition)
        {
            pPosition.GetChildVariable("X", &m_position.x);
            pPosition.GetChildVariable("Y", &m_position.y);
        }

        parser.GetChildVariable("Rotation", &m_rotation);

        auto pPrevPos = parser.GetChildElement("PreviousPos");
        if(pPrevPos)
        {
            pPrevPos.GetChildVariable("X", &m_prevPosition.x);
            pPrevPos.GetChildVariable("X", &m_prevPosition.y);
        }
    }

    void TransformComponent::Save(XMLElementParser parser)
    {
        auto pPosition = parser.InsertNewChild("Position");
        pPosition.NewChildVariable("X", m_position.x);
        pPosition.NewChildVariable("Y", m_position.y);

        parser.NewChildVariable("Rotation", m_rotation);

        if(m_prevPosition.x > 0 && m_prevPosition.y > 0)
        {
            auto pPrevPos = parser.InsertNewChild("PreviousPos");
            pPrevPos.NewChildVariable("X", m_prevPosition.x);
            pPrevPos.NewChildVariable("Y", m_prevPosition.y);
        }
        
    }
}