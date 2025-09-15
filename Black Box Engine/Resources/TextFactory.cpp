#include "TextFactory.h"

#include <SDL3_ttf/SDL_ttf.h>
#include <string>

#include "../BlackBoxManager.h"
#include "../Graphics/Text Rendering/Font.h"
#include "../Graphics/Text Rendering/Text.h"
#include "../Graphics/Text Rendering/TextRenderer.h"
#include "../System/StringHash.h"
#include "../System/Log.h"

namespace BlackBoxEngine
{
    TextFactory::HashType TextFactory::GetFontHash(const char* pFilePath, float fontSize)
    {
        HashType hash = StringHash(pFilePath);
        hash += static_cast<HashType>(fontSize);
        return hash;
    }

    std::shared_ptr<BB_Font> BlackBoxEngine::TextFactory::CreateFont(const char* pFontFilePath, float size)
    {
        HashType hash = GetFontHash(pFontFilePath, size);

        // find if already in cache
        auto it = m_fontCache.find(hash);
        if (it != m_fontCache.end())
        {
            auto& weakPtr = it->second;
            if (!weakPtr.expired())
                return weakPtr.lock();
            else
                m_fontCache.erase(it); // remove a dead pointer, then move onto making a new pointer.
        }

        // create new font if needed
        std::shared_ptr<BB_Font> retVal = std::make_shared<BB_Font>(pFontFilePath, size);
        auto emplacePair = m_fontCache.emplace(hash, retVal);

        // log any errors
        if (!retVal->m_pTtfFont)
            BB_LOG(LogType::kError, SDL_GetError(), "| font file path : ", pFontFilePath);
        else
            BB_LOG(LogType::kMessage, pFontFilePath, " imported properly");

        // I need to return a bad pointer if we fail.
        return retVal;
    }

    TextFactory::HashType BlackBoxEngine::TextFactory::GetTextHash(const char* pTextString, std::shared_ptr<BB_Font> pFont)
    {
        HashType textHash = StringHash(pTextString);
        HashType fontFileHash = StringHash(pFont->GetFontFilePath());
        HashType fontSizeHash = static_cast<HashType>(pFont->GetFontSize());

        return (textHash ^ (fontFileHash << 1) ^ (fontSizeHash << 1));
    }

    std::shared_ptr<BB_Text> TextFactory::CreateText(const char* pTextString, size_t stringLength, std::shared_ptr<BB_Font> pFont)
    {
        HashType hash = GetTextHash(pTextString, pFont);

        // find if already in cache
        auto it = m_textCache.find(hash);
        if (it != m_textCache.end())
        {
            auto& weakPtr = it->second;
            if (!weakPtr.expired())
                return weakPtr.lock();
            else
                m_textCache.erase(it); // remove a dead pointer, then move onto making a new pointer.
        }

        // create new text
        BB_TextRenderer* pRenderer = BlackBoxManager::Get()->GetWindow()->GetTextRenderer();
        std::shared_ptr<BB_Text> retVal = std::make_shared<BB_Text>(pTextString, stringLength, pFont, pRenderer);
        auto emplacePair = m_textCache.emplace(hash, retVal);

        if( ! retVal->m_pTtfText)
            BB_LOG(LogType::kError, SDL_GetError());

        return retVal;
    }

}