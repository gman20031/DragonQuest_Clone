#pragma once

#include <stdint.h>

#include "Color.h"
#include "RenderingStructs.h"

struct SDL_Texture;

namespace BlackBoxEngine
{

	/**
	 * @brief Reprsentation of a image file within the engine.
	 */
	class BB_Texture
	{
    public:
        enum class ScaleMode
        {
            kLinear,
            kNearest,
        };
        inline static constexpr ScaleMode kDefaultScaleMode = ScaleMode::kLinear;

    private:
		friend class BB_Renderer;
		SDL_Texture* m_pSdlTexture = nullptr;
	public:
		BB_Texture(SDL_Texture* pTexture);
		~BB_Texture();
		BB_Texture(const BB_Texture&) = delete;
		BB_Texture& operator=(const BB_Texture&) = delete;

        bool SetScaleMode(ScaleMode mode);
		bool SetAlpha(uint8_t alpha);
		bool SetColorMod(const ColorValue& colorMod);

        bool GetScaleMode(ScaleMode* pMode) const;
        bool GetAlpha(uint8_t* pAlpha) const;
        bool GetColorMod(ColorValue* pColor) const;
	};
}