#pragma once

#include <stdint.h>
#include <unordered_map>
#include <memory>

#include "LuaScript.h"
#include "PythonScript.h"

namespace BlackBoxEngine
{
    class ScriptFactory
    {
        friend class ResourceManager;

        using HashType = uint32_t;
        std::unordered_map<HashType, std::weak_ptr<LuaScript> > m_luaCache;
        //std::unordered_map<HashType, std::weak_ptr<PythonScript> > m_pythonCache;

        ScriptFactory() = default;

        std::shared_ptr<LuaScript> GetLuaScript(const char* pFilePath);
        //GetPythonScript(const char* pFilePath);
    };


}