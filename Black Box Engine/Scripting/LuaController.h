#pragma once

#include <memory>

#include "../libraries/Lua-4.5.7/src/lua.hpp"
#include "../Resources/ScriptResources/LuaScript.h"

namespace BlackBoxEngine
{
    class LuaController
    {
        lua_State* m_pLuaState = nullptr;

        LuaController();
    public:
        ~LuaController();

        void RunLuaFile(const char* fileName);
        void RunLuaScript(std::shared_ptr<LuaScript> luaScript);
        void RunLuaScript(const char* luaScript);

        void SetVariable(const char* varName, int value);
        void SetVariable(const char* varName, const char* value);

        int GetIntVariable(const char* varName);
        const char* GetStringVariable(const char* varName);

        void CallFunction(const char* varName);
        int CallFunctionIntReturn(const char* varName);

        void CreateTable(const char* tableName);
        void SetTableValue(const char* tableName, const char* key, int value);
        int  GetTableValue(const char* tableName, const char* key);

        static LuaController* Get();
    };

}
