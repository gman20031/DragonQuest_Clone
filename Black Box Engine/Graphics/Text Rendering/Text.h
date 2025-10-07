#pragma once

#include <memory>

#include "../RenderingStructs.h"
#include "../../Math/FVector2.h"
#include "../../Graphics/Color.h"

struct TTF_Text;

namespace BlackBoxEngine
{
    class BB_Font;

    class BB_Text
    {
        friend class TextFactory;
        friend class BB_TextRenderer;

        std::shared_ptr<BB_Font> m_pFont;
        BB_TextRenderer* m_pTextRenderer = nullptr;
        TTF_Text* m_pTtfText = nullptr;

        const char* m_pContents = nullptr;
        size_t m_contentsLength = 0;
    public:
        BB_Text(const char* contents, size_t contentsLength, std::shared_ptr<BB_Font> pFont, BB_TextRenderer* ptextRenderer);
        ~BB_Text();
        BB_Text(const BB_Text&) = delete;
        BB_Text(BB_Text&&) = delete;
        BB_Text& operator=(const BB_Text&) = delete;
        BB_Text& operator=(BB_Text&&) = delete;

        void SetColor(ColorValue color);
        void SetPosition(FVector2 position);
        void SetString(const char* pString, size_t length);
        bool SetTextWrapWidthPixels( int size );

        ColorValue GetColor() const;
        FVector2 GetPosition() const;
        const char* GetString() const;
        float GetPointSize() const;
        void GetPixelSize(int* pWidth, int* pHeight) const;
        int GetTextWrapWidthPixels() const;
        std::shared_ptr<BB_Font> GetFont() const;
    };

}