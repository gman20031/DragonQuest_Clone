#include "SpriteComponent.h"

#include <SDL.h>

#include "../ActorManager.h"
#include "../../Resources/ResourceManager.h"
#include "../../BlackBoxManager.h"
#include "../../System/Log.h"

namespace BlackBoxEngine
{
    SpriteComponent::SpriteComponent(Actor* pOwner)
        : Component(pOwner)
    {
        m_pTransform = FindOrCreateComponent<TransformComponent>(pOwner);
        m_pRenderer = BlackBoxManager::Get()->GetWindow()->GetRenderer();
    }

    void SpriteComponent::SetTexture(const char* pTexturePath)
    {
        assert(pTexturePath);
        m_pFilePath = pTexturePath;
        m_pTexture = ResourceManager::GetTexture(m_pRenderer, pTexturePath);
    }

    void SpriteComponent::SetDimensions(const float width, const float height)
    {
        m_width = width;
        m_height = height;
    }

    void SpriteComponent::Load(const XMLElementParser parser)
    {
        // renderer && pTranslator are both handled by ctor;
        
        parser.GetChildVariable("width",  &m_width);
        parser.GetChildVariable("height", &m_height);
        parser.GetChildVariable("filePath", &m_pFilePath);

        if (!m_pFilePath)
            BB_LOG(LogType::kFailure, "Bad texture file path");
        SetTexture(m_pFilePath);
    }

    void BlackBoxEngine::SpriteComponent::Save(XMLElementParser parser)
    {
        parser.NewChildVariable("width", m_width);
        parser.NewChildVariable("height", m_height);
        if (!m_pFilePath)
        {
            BB_LOG(LogType::kError, "Attempted to save sprite component with no texture");
            return;
        }
        parser.NewChildVariable("filePath", m_pFilePath);
    }

    void SpriteComponent::Render()
    {
        if (m_width == 0 || m_height == 0)
        {
            BB_LOG(LogType::kError, "Width or height is zero");
            return;
        }
        float x = m_pTransform->m_position.x;
        float y = m_pTransform->m_position.y;
        auto dest = BB_Rectangle(x , y , m_width, m_height);
        if ( !m_pRenderer->DrawTextureScreen(m_pTexture.get() , nullptr , &dest ))
            BB_LOG(LogType::kError, SDL_GetError());
    }
}
