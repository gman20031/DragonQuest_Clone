#include "Text.h"

#include <SDL3_ttf/SDL_ttf.h>

#include "Font.h"
#include "TextRenderer.h"

namespace BlackBoxEngine
{
    BB_Text::BB_Text(const char* contents, size_t contentsLength, std::shared_ptr<BB_Font> pFont, BB_TextRenderer* ptextRenderer)
        : m_pTextRenderer(ptextRenderer)
        , m_pFont(pFont)
    {
        m_pTtfText = TTF_CreateText(ptextRenderer->m_pTtfEngine, pFont->m_pTtfFont, contents, contentsLength);
    }

    BB_Text::~BB_Text()
    {
        TTF_DestroyText(m_pTtfText);
    }
    
    void BB_Text::SetColor(ColorValue color)
    {
        TTF_SetTextColor(m_pTtfText, color.r, color.g, color.b, color.a);
    }

    void BB_Text::SetPosition(FVector2 position)
    {
        TTF_SetTextPosition(m_pTtfText, (int)position.x, (int)position.y);
    }

    void BB_Text::SetString(const char* pString, size_t length)
    {
        TTF_SetTextString(m_pTtfText, pString, length);
    }

    bool BB_Text::SetTextWrapWidthPixels(int pixels)
    {
        return TTF_SetTextWrapWidth(m_pTtfText, pixels);
    }

    ColorValue BB_Text::GetColor() const
    {
        ColorValue color{};
        TTF_GetTextColor(m_pTtfText, &color.r, &color.g, &color.b, &color.a);
        return color;
    }

    FVector2 BB_Text::GetPosition() const
    {
        int x = 0, y = 0;
        TTF_GetTextPosition(m_pTtfText, &x, &y);
        return FVector2{ (float)x , (float)y };
    }

    const char* BB_Text::GetString() const
    {
        return m_pTtfText->text;
    }

    float BB_Text::GetPointSize() const
    {
        return TTF_GetFontSize(m_pFont->m_pTtfFont);
    }

    void BB_Text::GetPixelSize(int* pWidth, int* pHeight) const
    {
        TTF_GetTextSize(m_pTtfText, pWidth, pHeight);
    }

    int BB_Text::GetTextWrapWidthPixels() const
    {
        int w;
        TTF_GetTextWrapWidth(m_pTtfText, &w);
        return w;
    }

    std::shared_ptr<BB_Font> BB_Text::GetFont() const
    {
        return m_pFont;
    }

}
