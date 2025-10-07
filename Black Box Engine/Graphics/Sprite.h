#pragma once

#include <atomic>
#include <mutex>

#include "../Resources/ResourceManager.h"
#include "../Resources/XML/BlackBoxXMLParser.h"
#include "RenderingStructs.h"

namespace BlackBoxEngine
{
    class Sprite
    {
        const char* m_pFilePath = nullptr;
        std::shared_ptr<BB_Texture> m_pTexture = nullptr;
        BB_Renderer* m_pRenderer = nullptr;

        bool m_useFullImage = true;
        bool m_animating = false;

        std::atomic_int m_spriteSheetIndex = 0; // index should be able to be zero to wrap around
        int m_animStartIndex = 0;
        int m_animEndIndex = 0;
        BB_TRectangle<uint32_t> m_textureOffset{0,0,0,0};
        BB_TPoint<uint32_t> m_spriteDimensions{0,0};
        uint32_t m_spriteXCount = 0;
        uint32_t m_spriteYCount = 0;
        uint32_t m_spriteXPad = 0;
        uint32_t m_spriteYPad = 0;
        uint64_t m_callbackId = 0;
        int m_framesPerSecond = 0;
        bool m_animateOnStart = false;
        bool m_loopAnimation = false;
    private:
        void UpdateOffset();

    public:
        Sprite();
        ~Sprite();

        void AnimateSprite( int framePerSecond, bool repeat );
        void AnimateSprite();
        void StopAnimating();
        void Render( const BB_FRectangle& destRect, bool renderScreen = false );

        void SetTexture( const char* pFilePath);

        void UseFullImage() { m_useFullImage = true; }
        void SetSpriteIndex( int newIndex );
        void SetSpriteDimension( BB_TPoint<uint32_t> newDimensions );
        void SetSpriteXCount( uint32_t xCount );
        void SetSpriteYCount( uint32_t yCount );
        void SetSpriteXPad( uint32_t xPad );
        void SetSpriteYPad( uint32_t yPad );
        void SetAnimationStartIndex( int index );
        void SetAnimationEndIndex( int index );
        void SetFramesPerSecond( int frames ) { m_framesPerSecond = frames; };
        void SetAnimateOnStart( bool animStart ) { m_animateOnStart = animStart; };
        void SetLoopAnimation( bool loop ) { m_loopAnimation = loop; };

        int GetSpriteIndex() const { return m_spriteSheetIndex; }
        int GetAnimationStartIndex() const { return m_animStartIndex; }
        int GetAnimationEndIndex() const { return m_animEndIndex; }
        BB_TPoint<uint32_t> GetSpriteDimensions() const { return m_spriteDimensions; }
        uint32_t SetSpriteXCount() const { return m_spriteXCount; }
        uint32_t SetSpriteYCount() const { return m_spriteYCount; }
        uint32_t SetSpriteXPad() const { return m_spriteXPad; }
        uint32_t SetSpriteYPad() const { return m_spriteYPad; }
        int GetFramesPerSecond() const { return m_framesPerSecond; }
        bool GetAnimateOnStart() const { return m_animateOnStart; }
        bool GetIfAnimationLooping() const { return m_loopAnimation; }

        void Load( const XMLElementParser parser );
        void Save( XMLElementParser parser ) const;
        void GameStart();
    };

}