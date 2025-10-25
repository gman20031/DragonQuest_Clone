#pragma once

#include <memory>

#include "../Graphics/Renderer.h"

namespace BlackBoxEngine
{
    class XMLFactory;
    class XMLElementParser;
    class ActorXMLParser;
    class LevelXMLParser;

    class TextureFactory;
    class BB_Texture;

    class ScriptFactory;
    class LuaScript;

    class TextFactory;
    class BB_Font;
    class BB_Text;


    class ResourceManager
    {
        ResourceManager() = delete;

        inline static TextureFactory* GetTextureFactory();
        inline static XMLFactory* GetXMLFactory();
        inline static ScriptFactory* GetScriptFactory();
        inline static TextFactory* GetTextFactory();
    public:
        static std::shared_ptr<BB_Texture> GetTexture(BB_Renderer* pRenderer, const char* pFilepath);
        static std::shared_ptr<LuaScript> GetLuaScript(const char* pFilePath);
        static std::shared_ptr<BB_Font> GetFont(const char* pFilePath, float size);
        static std::shared_ptr<BB_Text> GetText(const char* pString, const char* pFontFilePath, float size);
        static std::shared_ptr<BB_Text> GetText(const char* pString, std::shared_ptr<BB_Font>);
        //static std::shared_ptr<PythonScript> GetPythonScript(const char* pFilePath);
        static ActorXMLParser GetActorXMLData(const char* pFilePath);
        static LevelXMLParser GetLevelXMLData(const char* pFilePath);
        static XMLElementParser GetRawXMLDATA(const char* pFilePath);

        static void RemoveXMLFromCache( const char* pFilePath );
    };



}