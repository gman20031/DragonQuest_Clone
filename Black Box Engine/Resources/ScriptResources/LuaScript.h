#pragma once

namespace BlackBoxEngine
{
    class LuaScript
    {
        const char* m_pRawScript = nullptr;
        size_t m_size = 0;
    public:
        LuaScript(const char* pRawScript, size_t size)
            : m_pRawScript(pRawScript)
            , m_size(size)
        { /*EMPTY*/ }

        ~LuaScript()
        {
            delete[] m_pRawScript;
        }

        const char* GetRaw() const { return m_pRawScript; }
    };

}
