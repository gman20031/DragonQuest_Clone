#include "ColliderDebugRenderer.h"

#include "../../BlackBoxManager.h"
#include "../../System/Debugging.h"
#include "../../Graphics/RenderingStructs.h"
#include "../Collision/AABBColliderComponent.h"

namespace BlackBoxEngine
{
    ColliderDebugRenderer::ColliderDebugRenderer(Actor* pActor)
        : Component(pActor)
    {
        m_pTransform = pActor->GetComponent<TransformComponent>();
        m_pRenderer = BlackBoxManager::Get()->GetWindow()->GetRenderer();
        m_pCollider = pActor->GetComponent<AABBColliderComponent>();
    }

    void ColliderDebugRenderer::Render()
    {
        if constexpr (kDebug)
        {
            BB_FRectangle rect = m_pCollider->GetCollider().m_rect;
            if (rect.w == 0 || rect.h == 0)
            {
                BB_LOG(LogType::kError, "Width or height is zero");
                return;
            }
            m_pRenderer->DrawRectScreenFilled(rect, m_renderColor);
        }
    }

    void ColliderDebugRenderer::Load(const XMLElementParser parser)
    {
        auto renderColor = parser.GetChildElement("RenderColor");

        renderColor.GetChildVariable("R", &m_renderColor.r);
        renderColor.GetChildVariable("G", &m_renderColor.g);
        renderColor.GetChildVariable("B", &m_renderColor.b);
        renderColor.GetChildVariable("A", &m_renderColor.a);
    }

    void ColliderDebugRenderer::Save(XMLElementParser parser)
    {
        auto renderColor = parser.InsertNewChild("RenderColor");

        renderColor.NewChildVariable("R", m_renderColor.r);
        renderColor.NewChildVariable("G", m_renderColor.g);
        renderColor.NewChildVariable("B", m_renderColor.b);
        renderColor.NewChildVariable("A", m_renderColor.a);
    }

}



