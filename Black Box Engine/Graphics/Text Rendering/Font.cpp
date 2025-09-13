#include "Font.h"

#include <cassert>
#include <SDL3_ttf/SDL_ttf.h>

namespace BlackBoxEngine
{
    BB_Font::BB_Font(const char* pFilePath, float size)
        : m_pTtfFont(TTF_OpenFont(pFilePath, size))
        , m_size(size)
        , m_pFilePath(pFilePath)
    {
    }

    BB_Font::~BB_Font()
    {
        assert(m_pTtfFont);
        TTF_CloseFont(m_pTtfFont);
    }
}
