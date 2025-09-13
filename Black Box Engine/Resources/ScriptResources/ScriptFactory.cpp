#include "ScriptFactory.h"

#include <fstream>

#include "../../System/StringHash.h"
#include "../../System/Log.h"

namespace BlackBoxEngine
{
    std::shared_ptr<LuaScript> ScriptFactory::GetLuaScript(const char* pFilePath)
    {
        HashType hash = StringHash(pFilePath);

        // find if already in cache.
        auto it = m_luaCache.find(hash);
        if (it != m_luaCache.end())
        {
            auto& weakPtr = it->second;
            if (!weakPtr.expired())
                return weakPtr.lock();
            else
                m_luaCache.erase(it);
        }

        // otherwise create new texture
        char* pRawScript = nullptr;
        std::fstream luaFile(pFilePath, std::ios::in | std::ios::ate);
        if (luaFile.bad() || !luaFile.is_open())
        {
            BB_LOG(LogType::kError, "Error opening file, filePath >> ", pFilePath);
            return std::make_shared<LuaScript>(nullptr, 0);
        }
        size_t size = luaFile.tellp();
        luaFile.seekg(0, std::ios::beg);
        pRawScript = new char[size+1];
        pRawScript[size] = '\0';
        luaFile.read(pRawScript, size);
        luaFile.close();
        assert(pRawScript);

        const auto& retVal = std::make_shared<LuaScript>(pRawScript, size);
        m_luaCache.emplace(hash, retVal);
        
        return retVal;
    }

}
