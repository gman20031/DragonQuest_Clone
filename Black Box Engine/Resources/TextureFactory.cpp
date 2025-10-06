#include "TextureFactory.h"

#include <SDL.h>
#include <SDL_image.h>

#include "../System/StringHash.h"
#include "../System/Log.h"
#include "../Graphics/Renderer.h"

namespace BlackBoxEngine
{
    std::shared_ptr<BB_Texture>
        TextureFactory::CreateTexture(const BB_RendererPtr pRenderer, const char* pImageFilePath)
    {
        HashType hash = StringHash(pImageFilePath);

        // find if already in cache
        auto it = m_textureCache.find(hash);
        if (it != m_textureCache.end())
        {
            auto& weakPtr = it->second;
            if (!weakPtr.expired())
                return weakPtr.lock();
            else
                m_textureCache.erase(it);
        }

        // create new texture
        auto* pSDLTexture = IMG_LoadTexture(pRenderer->m_pSdlRenderer, pImageFilePath);
        std::shared_ptr<BB_Texture> retVal = std::make_shared<BB_Texture>(pSDLTexture);
        auto emplacePair = m_textureCache.emplace(hash, retVal);

        // log errors
        if (!emplacePair.second || !retVal)
            BB_LOG(LogType::kError, SDL_GetError(), " texture file path : ", pImageFilePath);
        else
            BB_LOG(LogType::kMessage, pImageFilePath, " imported properly");

        // return new or faulty
        return retVal;
    }
}
