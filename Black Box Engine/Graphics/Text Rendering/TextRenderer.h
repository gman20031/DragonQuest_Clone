#pragma once

#include <memory>

#include "../../Math/FVector2.h"
#include "../RenderingStructs.h"
#include "../Color.h"
#include "../Window.h"
#include "../Texture.h"

struct TTF_TextEngine;

namespace BlackBoxEngine
{
    class BB_Camera;
    class BB_Renderer;
    /**
     * @brief Is the pText equivilant of the BB_Renderer class
     * @brief Where the Texture render pipeline is:
     * @brief   - Get Renderer + Get Picture File -> Texture
     * @brief   - Use Renderer to display Texture
     * @brief this does the same but with Fonts and Texts instead of Pictures and textures
     */
    class BB_TextRenderer
    {
        friend class BB_Text;
        friend class BB_Font;
    private:
        const BB_Window* m_pAttachedWindow;
        const BB_Camera* m_pGameCamera = nullptr;
        TTF_TextEngine* m_pTtfEngine = nullptr;

    public:
        BB_TextRenderer(BB_Window* pWindow);
        ~BB_TextRenderer();

        BB_Renderer* GetStandardRenderer() const;

        bool DrawTextBlended(std::shared_ptr<BB_Font> pFont, const char* pText, size_t length, ColorValue foregroundColor, BB_FRectangle destination);
        bool DrawTextSolid(std::shared_ptr<BB_Font> pFont, const char* pText, size_t length, ColorValue foregroundColor, BB_FRectangle destination);
        bool DrawTextShaded(std::shared_ptr<BB_Font> pFont, const char* pText, size_t length, ColorValue foregroundColor, ColorValue backgroundColor, BB_FRectangle destination);
        bool DrawTextLCD(std::shared_ptr<BB_Font> pFont, const char* pText, size_t length, ColorValue foregroundColor, ColorValue backgroundColor, BB_FRectangle destination);

        bool RenderText(std::shared_ptr<BB_Text> text, float x, float y);

        void RenderTestAll(std::shared_ptr<BB_Text> pText);
    };

}
