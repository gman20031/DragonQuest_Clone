#include "LuaController.h"

#include "../System/Log.h"
#include <cassert>

namespace BlackBoxEngine
{

    LuaController::LuaController()
    {
        m_pLuaState = luaL_newstate();
        luaL_openlibs(m_pLuaState);
    }

    LuaController::~LuaController()
    {
        assert(m_pLuaState);
        lua_close(m_pLuaState);
    }

    void LuaController::RunLuaFile(const char* fileName)
    {
        bool state = luaL_dofile(m_pLuaState, fileName);
        if (state != LUA_OK)
        {
            BB_LOG(LogType::kError, "Lua Error: ", lua_tostring(m_pLuaState, 1), '\n');
            lua_pop(m_pLuaState, 1);
        }
    }

    void LuaController::RunLuaScript(std::shared_ptr<LuaScript> luaScript)
    {
        RunLuaScript(luaScript->GetRaw());
    }

    void LuaController::RunLuaScript(const char* luaScript)
    {
        bool state = luaL_loadstring(m_pLuaState, luaScript);
        if (state != LUA_OK)
        {
            BB_LOG(LogType::kError, "Lua Error: ", lua_tostring(m_pLuaState, 1), '\n');
            lua_pop(m_pLuaState, 1);
            return;
        }

        state = lua_pcall(m_pLuaState, 0, 0, 0);
        if (state != LUA_OK)
        {
            BB_LOG(LogType::kError, "Lua Error: ", lua_tostring(m_pLuaState, 1), '\n');
            lua_pop(m_pLuaState, 1);
            return;
        }

        return;
    }

    void LuaController::SetVariable(const char* varName, int value)
    {
        lua_pushinteger(m_pLuaState, value);
        lua_setglobal(m_pLuaState, varName);
    }

    void LuaController::SetVariable(const char* varName, const char* value)
    {
        lua_pushstring(m_pLuaState, value);
        lua_setglobal(m_pLuaState, varName);
    }

    int LuaController::GetIntVariable(const char* varName)
    {
        lua_getglobal(m_pLuaState, varName);
        int val = static_cast<int>(lua_tointeger(m_pLuaState, -1));
        lua_pop(m_pLuaState, 1);

        return val;
    }

    const char* LuaController::GetStringVariable(const char* varName)
    {
        lua_getglobal(m_pLuaState, varName);
        const char* val = lua_tostring(m_pLuaState, -1);
        lua_pop(m_pLuaState, 1);

        return val;
    }

    void LuaController::CallFunction(const char* varName)
    {
        lua_getglobal(m_pLuaState, varName);
        bool state = lua_pcall(m_pLuaState, 0, 0, 0);
        if (state != LUA_OK)
        {
            BB_LOG(LogType::kError, "Lua Error: ", lua_tostring(m_pLuaState, 1), '\n');
            lua_pop(m_pLuaState, 1);
        }
    }

    int LuaController::CallFunctionIntReturn(const char* varName)
    {
        lua_getglobal(m_pLuaState, varName);
        bool state = lua_pcall(m_pLuaState, 0, 0, 0);
        if (state != LUA_OK)
        {
            BB_LOG(LogType::kError, "Lua Error: ", lua_tostring(m_pLuaState, 1), '\n');
            lua_pop(m_pLuaState, 1);
        }

        int result = static_cast<int>( lua_tointeger(m_pLuaState, -1) );
        lua_pop(m_pLuaState, 1);

        return result;
    }

    void LuaController::CreateTable(const char* tableName)
    {
        lua_newtable(m_pLuaState);
        lua_setglobal(m_pLuaState, tableName);
    }

    void LuaController::SetTableValue(const char* tableName, const char* key, int value)
    {
        lua_getglobal(m_pLuaState, tableName);
        bool state = lua_istable(m_pLuaState, -1);
        if (state != LUA_OK)
        {
            lua_pop(m_pLuaState, 1);
            BB_LOG(LogType::kError, "Lua Error: ", lua_tostring(m_pLuaState, 1), '\n');
            return;
        }

        lua_pushstring(m_pLuaState, key);
        lua_pushinteger(m_pLuaState, value);
        lua_settable(m_pLuaState, -3);
        lua_pop(m_pLuaState, 1);
    }

    int LuaController::GetTableValue(const char* tableName, const char* key)
    {
        lua_getglobal(m_pLuaState, tableName);
        lua_pushstring(m_pLuaState, key);
        lua_gettable(m_pLuaState, -2);
        int val = static_cast<int> ( lua_tointeger(m_pLuaState, -1) );
        lua_pop(m_pLuaState, 2);

        return val;
    }

    LuaController* LuaController::Get()
    {
        static LuaController controller;
        return &controller;
    }

}