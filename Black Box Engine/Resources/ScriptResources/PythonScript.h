#pragma once

namespace BlackBoxEngine
{
    class PythonScript
    {
        const char* m_pRawScript = nullptr;
        size_t m_size = 0;
    public:
        PythonScript(const char* pRawScript, size_t size)
            : m_pRawScript(pRawScript)
            , m_size(size)
        { /*EMPTY*/ }

        ~PythonScript()
        {
            delete[] m_pRawScript;
        }

        const char* GetRaw() const { return m_pRawScript; }
    };

}