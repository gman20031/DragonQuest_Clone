#pragma once

#include <memory>
struct TTF_Font;

namespace BlackBoxEngine
{
    class BB_Font
    {
    private:
        friend class BB_Text;
        friend class BB_TextRenderer;
        friend class TextFactory;

        TTF_Font* m_pTtfFont = nullptr;
        const char* m_pFilePath = nullptr;
        float m_size;

    public:
        BB_Font(const char* pFilePath, float size);
        ~BB_Font();
        BB_Font(const BB_Font&) = delete;
        BB_Font(BB_Font&&) = delete;
        BB_Font& operator=(const BB_Font&) = delete;
        BB_Font& operator=(BB_Font&&) = delete;

        float GetFontSize() const { return m_size; }
        const char* GetFontFilePath() const { return m_pFilePath; }
    };
}

