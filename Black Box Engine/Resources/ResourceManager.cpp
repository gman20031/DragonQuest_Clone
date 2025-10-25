#include "ResourceManager.h"

#include <cassert>

#include "XML/BlackBoxXMLParser.h"
#include "../Graphics/Renderer.h"
#include "TextureFactory.h"
#include "XML/XMLFactory.h"
#include "ScriptResources/ScriptFactory.h"
#include "TextFactory.h"

namespace BlackBoxEngine
{
    inline TextureFactory* ResourceManager::GetTextureFactory()
    {
        static TextureFactory texFactory; //= new TextureFactory;
        return &texFactory;
    }

    inline XMLFactory* ResourceManager::GetXMLFactory()
    {
        static XMLFactory xmlFactory; //= new XMLFactory;
        return &xmlFactory;
    }

    inline ScriptFactory* ResourceManager::GetScriptFactory()
    {
        static ScriptFactory scriptFactory; //= new scriptFactory;
        return &scriptFactory;
    }

    inline TextFactory* ResourceManager::GetTextFactory()
    {
        static TextFactory textFactory;
        return &textFactory;
    }

    std::shared_ptr<BB_Texture> ResourceManager::GetTexture(BB_Renderer* pRenderer, const char* pFilepath)
    {
        return GetTextureFactory()->CreateTexture(pRenderer, pFilepath);
    }

    std::shared_ptr<LuaScript> ResourceManager::GetLuaScript(const char* pFilePath)
    {
        return GetScriptFactory()->GetLuaScript(pFilePath);
    }

    ActorXMLParser ResourceManager::GetActorXMLData(const char* pFilePath)
    {
        return GetXMLFactory()->CreateActorFromFile(pFilePath);
    }

    LevelXMLParser ResourceManager::GetLevelXMLData(const char* pFilePath)
    {
        return GetXMLFactory()->CreateLevelFromFile(pFilePath);
    }

    XMLElementParser ResourceManager::GetRawXMLDATA(const char* pFilePath)
    {
        return GetXMLFactory()->CreateXMLFile(pFilePath);
    }

    void ResourceManager::RemoveXMLFromCache( const char* pFilePath )
    {
        GetXMLFactory()->RemoveFromCache( pFilePath );
    }

    std::shared_ptr<BB_Font> ResourceManager::GetFont(const char* pFilePath, float size)
    {
        return GetTextFactory()->CreateFont(pFilePath, size);
    }

    std::shared_ptr<BB_Text> ResourceManager::GetText(const char* pString, const char* pFontFilePath, float size)
    {
        assert(pString);
        std::shared_ptr<BB_Font> pFont = GetFont(pFontFilePath, size);
        return GetTextFactory()->CreateText(pString, std::strlen(pString) , pFont  );
    }

    std::shared_ptr<BB_Text> ResourceManager::GetText(const char* pString, std::shared_ptr<BB_Font> pFont)
    {
        assert(pString);
        return GetTextFactory()->CreateText(pString, std::strlen(pString), pFont);
    }

}

