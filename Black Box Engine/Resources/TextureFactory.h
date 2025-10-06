#pragma once

#include <memory>
#include <unordered_map>

#include "../Graphics/Renderer.h"

namespace BlackBoxEngine
{
	/**
	 * @brief Pure static factory for BB_Textures, handles caching to stop
	 * duplication of image files.
	 */
	class TextureFactory
	{
        friend class ResourceManager;
		using HashType = uint32_t;
		using BB_RendererPtr = BB_Renderer*;

		std::unordered_map<HashType, std::weak_ptr<BB_Texture> > m_textureCache;
        std::shared_ptr<BB_Texture> CreateTexture(const BB_RendererPtr pRenderer, const char* imageFilePath);
    };

}