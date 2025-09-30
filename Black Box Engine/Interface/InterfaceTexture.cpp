#include "InterfaceTexture.h"

namespace BlackBoxEngine
{
    void BlackBoxEngine::InterfaceTexture::RenderThis([[maybe_unused]] BB_Renderer* pRenderer , float rootX, float rootY)
    {
        BB_FRectangle dest = m_nodeRenderRect;
        dest.x = m_nodeRenderRect.x + rootX;
        dest.y = m_nodeRenderRect.y + rootY;
        m_pSprite->Render( dest );
    }

    InterfaceTexture::InterfaceTexture( InterfaceNode* pParent, const char* pName, BB_FRectangle dimensions, const TextureInfo& params )
        : InterfaceNode( pParent , pName, dimensions )
        , m_params(params)
    {
        m_pSprite = new Sprite;
        if( ! params.useFullImage )
        {
            m_pSprite->SetSpriteDimension( params.spriteDimensions );
            m_pSprite->SetSpriteXCount( params.spriteXCount );
            m_pSprite->SetSpriteYCount( params.spriteYCount );
            m_pSprite->SetSpriteXPad( params.spriteXPad );
            m_pSprite->SetSpriteYPad( params.spriteXPad );
            m_pSprite->SetSpriteIndex( params.spriteSheetIndex );
        }
    }

    InterfaceTexture::~InterfaceTexture()
    {
        assert( m_pSprite );
        delete m_pSprite;
    }
}

