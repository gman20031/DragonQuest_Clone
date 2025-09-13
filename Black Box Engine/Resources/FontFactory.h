#pragma once

#include <memory>
#include <unordered_map>

#include "../Graphics/Renderer.h"


namespace BlackBoxEngine
{
    class BB_Font;
    class BB_Text;

    class TextFactory
    {
        friend class ResourceManager;
        using HashType = uint32_t;
    
        HashType GetFontHash(const char* pFilePath, float fontSize);
        HashType GetTextHash(const char* pTextString, std::shared_ptr<BB_Font> pFont);
        std::unordered_map<HashType, std::weak_ptr<BB_Font> > m_fontCache;
        std::unordered_map<HashType, std::weak_ptr<BB_Text> > m_textCache;

        std::shared_ptr<BB_Font> CreateFont(const char* pFontFilePath, float size);
        std::shared_ptr<BB_Text> CreateText(const char* pTextString, size_t stringLength, std::shared_ptr<BB_Font> pFont);
    };
}
