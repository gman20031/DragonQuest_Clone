#pragma once

#include "../Graphics/Sprite.h"
#include "../Graphics/Color.h"
#include "../Math/FVector2.h"
#include "UserInterface.h"

namespace BlackBoxEngine
{
    class InterfaceTexture : public InterfaceNode
    {
    public:
        Sprite* m_pSprite;

        struct TextureInfo
        {
            const char* pTextureFile = nullptr;
            BB_TPoint<uint32_t> spriteDimensions{0,0};
            bool useFullImage = true;
            bool animate = false;
            uint32_t spriteXCount = 0;
            uint32_t spriteYCount = 0;
            uint32_t spriteXPad = 0;
            uint32_t spriteYPad = 0;
            uint32_t animationStartIndex = 0;
            uint32_t animationEndIndex = 0;
            int spriteSheetIndex = 0;
            int framesPerSecond = 0;
            float repeat = false;
        };
    private:
        TextureInfo m_params;
    
        virtual void RenderThis( [[maybe_unused]] BB_Renderer* pRenderer, float rootX, float rootY ) override;
    public:
        InterfaceTexture(InterfaceNode* pParent, const char* pName, BB_FRectangle dimensions, const TextureInfo& params);
        virtual ~InterfaceTexture() override;

        const Sprite* GetSprite() const { return m_pSprite; }
        Sprite* Sprite() { return m_pSprite; }
    };
}
