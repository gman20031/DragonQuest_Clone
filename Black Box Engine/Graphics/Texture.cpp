#include "Texture.h"

#include <SDL.h>
#include <SDL_image.h>

#include "../System/Log.h"

namespace BlackBoxEngine
{
    BB_Texture::BB_Texture(SDL_Texture* pTexture)
	    : m_pSdlTexture(pTexture)
    {
	    if (!m_pSdlTexture)
		    BB_LOG(LogType::kError , "Texture not found when wrapping SDL_Texture");

        SetScaleMode(kDefaultScaleMode);
    }

    BB_Texture::~BB_Texture()
    {
	    if(m_pSdlTexture)
		    SDL_free(m_pSdlTexture);
    }

    bool BB_Texture::SetScaleMode(ScaleMode mode)
    {
        return SDL_SetTextureScaleMode(m_pSdlTexture, static_cast<SDL_ScaleMode>(mode));
    }

    bool BB_Texture::SetAlpha(uint8_t alpha)
    {
	    return SDL_SetTextureAlphaMod(m_pSdlTexture, alpha);
    }

    bool BB_Texture::SetColorMod(const ColorValue& colorMod)
    {
	    return SDL_SetTextureColorMod(m_pSdlTexture, colorMod.r, colorMod.g, colorMod.b);
    }

    bool BB_Texture::GetScaleMode(ScaleMode* pMode) const
    {
        SDL_ScaleMode mode;
        bool good = SDL_GetTextureScaleMode(m_pSdlTexture, &mode);
        *pMode = static_cast<ScaleMode>(mode);
        return good;
    }

    bool BB_Texture::GetAlpha(uint8_t* pAlpha) const
    {
        return SDL_GetTextureAlphaMod(m_pSdlTexture, pAlpha);
    }

    bool BB_Texture::GetColorMod(ColorValue* pColor) const
    {
        bool good = SDL_GetTextureColorMod(m_pSdlTexture, &pColor->r, &pColor->g, &pColor->b);
        return good;
    }

}


