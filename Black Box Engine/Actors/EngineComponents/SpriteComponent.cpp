#include "SpriteComponent.h"

#include <SDL.h>

#include "../ActorManager.h"
#include "../../Resources/ResourceManager.h"
#include "../../BlackBoxManager.h"
#include "../../System/Log.h"

namespace BlackBoxEngine
{
    AnimatedSpriteComponent::AnimatedSpriteComponent( Actor* pOwner )
        : Component( pOwner )
    {
        m_pTransform = FindOrCreateComponent<TransformComponent>( pOwner );
        m_pRenderer = BlackBoxManager::Get()->GetWindow()->GetRenderer();
    }

    void AnimatedSpriteComponent::SetDrawnDimensions( const float width, const float height )
    {
        m_drawnDimensions.x = width;
        m_drawnDimensions.y = height;
    }

    void AnimatedSpriteComponent::SetDrawnDimensions( const FVector2 newDimensions )
    {
        m_drawnDimensions = newDimensions;
    }

    void AnimatedSpriteComponent::SetOffset( const float xOff, const float yOff )
    {
        m_offset.x = xOff;
        m_offset.y = yOff;
    }

    void AnimatedSpriteComponent::SetOffset( const FVector2 offset )
    {
        m_offset = offset;
    }

    void AnimatedSpriteComponent::Load( const XMLElementParser parser )
    {
        parser.GetChildVariable( "DrawnWidth", &m_drawnDimensions.x );
        parser.GetChildVariable( "DrwanHeight", &m_drawnDimensions.y );
        parser.GetChildVariable( "xOffset", &m_offset.x );
        parser.GetChildVariable( "yOffset", &m_offset.y );

        auto spriteParser = parser.GetChildElement( "SpriteInfo" );
        m_sprite.Load( spriteParser );
    }

    void BlackBoxEngine::AnimatedSpriteComponent::Save( XMLElementParser parser )
    {
        parser.NewChildVariable( "DrawnWidth", m_drawnDimensions.x );
        parser.NewChildVariable( "DrwanHeight", m_drawnDimensions.y );
        parser.NewChildVariable( "xOffset", m_offset.x );
        parser.NewChildVariable( "yOffset", m_offset.y );

        auto spriteParser = parser.InsertNewChild( "SpriteInfo" );
        m_sprite.Save( spriteParser );
    }

    void AnimatedSpriteComponent::Render()
    {
        if ( m_drawnDimensions.x == 0 || m_drawnDimensions.y == 0)
        {
            BB_LOG(LogType::kError, "Width or height is zero");
            return;
        }
        float x = m_pTransform->m_position.x + m_offset.x;
        float y = m_pTransform->m_position.y + m_offset.y;
        auto dest = BB_FRectangle(x , y , m_drawnDimensions.x , m_drawnDimensions.y);

        m_sprite.Render( dest );
    }

    void AnimatedSpriteComponent::Start()
    {
        m_sprite.GameStart();
    }

    /////////////////////////////////////////////////////////////////
    /// Original SpriteComponent
    /////////////////////////////////////////////////////////////////

    SpriteComponent::SpriteComponent( Actor* pOwner )
        : Component( pOwner )
    {
        m_pTransform = FindOrCreateComponent<TransformComponent>( pOwner );
        m_pRenderer = BlackBoxManager::Get()->GetWindow()->GetRenderer();
    }

    void SpriteComponent::SetTexture( const char* pTexturePath )
    {
        assert( pTexturePath );
        m_pFilePath = pTexturePath;
        m_pTexture = ResourceManager::GetTexture( m_pRenderer, pTexturePath );
    }

    void SpriteComponent::SetTextureOffset( BB_FRectangle offset )
    {
        m_useFullImage = false;
        m_textureOffset = offset;
    }

    void SpriteComponent::SetDimensions( const float width, const float height )
    {
        m_width = width;
        m_height = height;
    }

    void SpriteComponent::Load( const XMLElementParser parser )
    {
        // renderer && pTranslator are both handled by ctor;

        parser.GetChildVariable( "width", &m_width );
        parser.GetChildVariable( "height", &m_height );
        parser.GetChildVariable( "UseFullImage", &m_useFullImage );
        if ( !m_useFullImage )
        {
            auto element = parser.GetChildElement( "TextureOffset" );
            element.GetChildVariable( "X", &m_textureOffset.x );
            element.GetChildVariable( "Y", &m_textureOffset.y );
            element.GetChildVariable( "W", &m_textureOffset.w );
            element.GetChildVariable( "H", &m_textureOffset.h );
        }

        parser.GetChildVariable( "filePath", &m_pFilePath );

        if ( !m_pFilePath )
            BB_LOG( LogType::kFailure, "Bad texture file path" );
        SetTexture( m_pFilePath );
    }

    void BlackBoxEngine::SpriteComponent::Save( XMLElementParser parser )
    {
        parser.NewChildVariable( "width", m_width );
        parser.NewChildVariable( "height", m_height );
        parser.NewChildVariable( "UseFullImage", m_useFullImage );
        if ( !m_useFullImage )
        {
            auto element = parser.InsertNewChild( "TextureOffset" );
            element.NewChildVariable( "X", m_textureOffset.x );
            element.NewChildVariable( "Y", m_textureOffset.y );
            element.NewChildVariable( "W", m_textureOffset.w );
            element.NewChildVariable( "H", m_textureOffset.h );
        }
        if ( !m_pFilePath )
        {
            BB_LOG( LogType::kError, "Attempted to save sprite component with no texture" );
            return;
        }
        parser.NewChildVariable( "filePath", m_pFilePath );
    }

    void SpriteComponent::Render()
    {
        if ( m_width == 0 || m_height == 0 )
        {
            BB_LOG( LogType::kError, "Width or height is zero" );
            return;
        }
        float x = m_pTransform->m_position.x;
        float y = m_pTransform->m_position.y;
        auto dest = BB_FRectangle( x, y, m_width, m_height );

        BB_FRectangle* pDest = nullptr;
        if ( !m_useFullImage )
            pDest = &m_textureOffset;

        if ( !m_pRenderer->DrawTextureGame(
            m_pTexture.get(),
            pDest,
            &dest ) )
        {
            BB_LOG( LogType::kError, SDL_GetError() );
        }
    }

}
