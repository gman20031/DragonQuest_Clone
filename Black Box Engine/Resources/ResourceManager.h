#pragma once

#include <memory>

#include "TextureFactory.h"
#include "XML/DataFactory.h"
#include "ScriptResources/ScriptFactory.h"

namespace BlackBoxEngine
{
    class ResourceManager
    {
        ResourceManager() = delete;

        inline static TextureFactory* GetTextureFactory();
        inline static XMLFactory* GetXMLFactory();
        inline static ScriptFactory* GetScriptFactory();

    public:
        static std::shared_ptr<BB_Texture> GetTexture(BB_Renderer* pRenderer, const char* pFilepath);
        static std::shared_ptr<LuaScript> GetLuaScript(const char* pFilePath);
        //static std::shared_ptr<PythonScript> GetPythonScript(const char* pFilePath);
        static ActorXMLParser GetActorXMLData(const char* pFilePath);
        static LevelXMLParser GetLevelXMLData(const char* pFilePath);

    };



}