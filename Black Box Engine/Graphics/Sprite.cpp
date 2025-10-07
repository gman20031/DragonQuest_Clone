#include "Sprite.h"

#include <SDL3/SDL.h>
#include <cassert>

#include "../BlackBoxManager.h"
#include "../System/Delay.h"
#include "../System/Log.h"

namespace BlackBoxEngine
{
    void Sprite::UpdateOffset()
    {
        if ( m_spriteXCount == 0 || m_spriteYCount == 0 )
            return;
        m_textureOffset.w = m_spriteDimensions.x;
        m_textureOffset.h = m_spriteDimensions.y;
        const int spriteX = (m_spriteSheetIndex % (m_spriteXCount));
        const int spriteY = (m_spriteSheetIndex / (m_spriteXCount));
        m_textureOffset.x = spriteX * (m_spriteDimensions.x) + (m_spriteXPad * spriteX);
        m_textureOffset.y = spriteY * (m_spriteDimensions.y) + (m_spriteYPad * spriteY);
    }

    Sprite::Sprite()
    {
        m_pRenderer = BlackBoxManager::Get()->GetWindow()->GetRenderer();
    }

    Sprite::~Sprite()
    {
        StopAnimating();
    }

    void Sprite::AnimateSprite( int framePerSecond, bool repeat )
    {
        if ( m_animating )
            StopAnimating();

        constexpr uint64_t milisecondPerSecond = std::chrono::milliseconds::period::den;
        double frameDelay = 1 / (double)framePerSecond;
        double msDelay = (frameDelay * milisecondPerSecond);

        int totalFrameCount = m_spriteXCount * m_spriteYCount;
        m_spriteSheetIndex = m_animStartIndex;
        m_animating = true;

        SetSpriteIndex( m_animStartIndex );
        auto callback = [msDelay, totalFrameCount, repeat, this]() -> void
            {
                int spriteIndex = GetSpriteIndex();
                ++spriteIndex;
                if ( spriteIndex >= totalFrameCount || spriteIndex > m_animEndIndex)
                    spriteIndex = m_animStartIndex;
                assert( spriteIndex >= 0 && spriteIndex <= totalFrameCount );
                SetSpriteIndex( spriteIndex );
            };
        double repeatDelay = 0;
        if ( repeat )
            repeatDelay = msDelay;
        m_callbackId = DelayedCallbackManager::AddCallback(callback, msDelay, repeatDelay);
    }

    void Sprite::AnimateSprite()
    {
        AnimateSprite( m_framesPerSecond, m_loopAnimation );
    }

    void Sprite::StopAnimating()
    {
        if ( !m_animating )
            return;
        m_animating = false;
        if ( ! DelayedCallbackManager::RemoveCallback(m_callbackId) )
            BB_LOG( LogType::kWarning, "error when attempting to remove animation callback, SDL: ", SDL_GetError() );
    }

    void Sprite::Render(const BB_FRectangle& destRect , bool renderScreen )
    {
        BB_FRectangle source = {};
        BB_FRectangle* pSource = nullptr;
        if ( !m_useFullImage )
        {
            if ( m_spriteDimensions.x <= 0 || m_spriteDimensions.y == 0 )
            {
                BB_LOG( LogType::kError, "Attempted to render sprite with either 0 height or width" );
                return;
            }
            source.x = static_cast<float>(m_textureOffset.x);
            source.y = static_cast<float>(m_textureOffset.y);
            source.w = static_cast<float>(m_textureOffset.w);
            source.h = static_cast<float>(m_textureOffset.h);
            pSource = &source;
        }

        bool good = false;
        if ( renderScreen )
            good = m_pRenderer->DrawTextureScreen( m_pTexture.get(), pSource, &destRect );
        else
            good = m_pRenderer->DrawTextureGame(m_pTexture.get(), pSource, &destRect );
        if (!good)
            BB_LOG( LogType::kError, SDL_GetError() );
    }

    void Sprite::SetTexture( const char* pTexturePath )
    {
        assert( pTexturePath );
        m_pFilePath = pTexturePath;
        m_pTexture = ResourceManager::GetTexture( m_pRenderer, pTexturePath );
    }

    void Sprite::SetSpriteIndex( int newIndex )
    {
        assert( newIndex <= static_cast<int>(m_spriteXCount * m_spriteYCount) && newIndex >= 0);
        m_useFullImage = false;
        m_spriteSheetIndex = newIndex;
        UpdateOffset();
    }

    void Sprite::SetSpriteDimension( BB_TPoint<uint32_t> newDimensions )
    {
        assert( newDimensions.x > 0 && newDimensions.y > 0 );
        m_spriteDimensions = newDimensions;
        UpdateOffset();
    }

    void Sprite::SetSpriteXCount( uint32_t xCount )
    {
        m_spriteXCount = xCount;
        UpdateOffset();
    }

    void Sprite::SetSpriteYCount( uint32_t yCount )
    {
        m_spriteYCount = yCount;
        UpdateOffset();
    }

    void Sprite::SetSpriteXPad( uint32_t xPad )
    {
        m_spriteXPad = xPad;
        UpdateOffset();
    }

    void Sprite::SetSpriteYPad( uint32_t yPad )
    {
        m_spriteYPad = yPad;
        UpdateOffset();
    }

    void Sprite::SetAnimationStartIndex( int index )
    {
        assert( index >= 0 && index < static_cast<int>(m_spriteXCount * m_spriteYCount) );
        m_animStartIndex = index;
    }

    void Sprite::SetAnimationEndIndex( int index )
    {
        assert( index >= 0 && index < static_cast<int>(m_spriteXCount * m_spriteYCount) );
        m_animEndIndex = index;
    }

    void Sprite::Load( const XMLElementParser parser )
    {
        int tempIndex;
        parser.GetChildVariable( "startingSpriteIndex", &tempIndex );
        m_spriteSheetIndex = tempIndex;

        parser.GetChildVariable( "UseFullImage", &m_useFullImage );
        if ( !m_useFullImage )
        {
            auto spriteDim = parser.GetChildElement( "spriteDimensions" );
            spriteDim.GetChildVariable( "width", &m_spriteDimensions.x );
            spriteDim.GetChildVariable( "height", &m_spriteDimensions.y );
        }
        parser.GetChildVariable( "filePath", &m_pFilePath );

        parser.GetChildVariable( "sprite_x_count", &m_spriteXCount );
        parser.GetChildVariable( "sprite_y_count", &m_spriteYCount );
        parser.GetChildVariable( "sprite_x_pad", &m_spriteXPad );
        parser.GetChildVariable( "sprite_y_pad", &m_spriteXPad );
        parser.GetChildVariable( "anim_start_index", &m_animStartIndex );
        parser.GetChildVariable( "anim_end_index", &m_animEndIndex );
        parser.GetChildVariable( "frames_per_second", &m_framesPerSecond );
        parser.GetChildVariable( "looping", &m_loopAnimation );
        parser.GetChildVariable( "animated_on_start", &m_animateOnStart );

        SetTexture( m_pFilePath );
        UpdateOffset();
    }

    void Sprite::Save( XMLElementParser parser ) const
    {
        parser.NewChildVariable( "startingSpriteIndex", m_spriteSheetIndex );

        parser.NewChildVariable( "UseFullImage", m_useFullImage );
        if ( !m_useFullImage )
        {
            auto spriteDim = parser.InsertNewChild( "spriteDimensions" );
            spriteDim.NewChildVariable( "width", (float)m_spriteDimensions.x );
            spriteDim.NewChildVariable( "height", (float)m_spriteDimensions.y);
        }
        if ( !m_pFilePath )
        {
            BB_LOG( LogType::kError, "Attempted to save sprite with no texture" );
            return;
        }
        parser.NewChildVariable( "filePath", m_pFilePath );

        parser.NewChildVariable( "sprite_x_count", m_spriteXCount );
        parser.NewChildVariable( "sprite_y_count", m_spriteYCount );
        parser.NewChildVariable( "sprite_x_pad", m_spriteXPad );
        parser.NewChildVariable( "sprite_y_pad", m_spriteXPad );
        parser.NewChildVariable( "anim_start_index", m_animStartIndex );
        parser.NewChildVariable( "anim_end_index", m_animEndIndex );
        parser.NewChildVariable( "frames_per_second", m_framesPerSecond );
        parser.NewChildVariable( "looping", m_loopAnimation );
        parser.NewChildVariable( "animated_on_start", m_animateOnStart );
    }

    void Sprite::GameStart( )
    {
        if( m_animateOnStart )
            AnimateSprite( m_framesPerSecond, m_loopAnimation );
    }
}