#include "LuaScriptComponent.h"

#include "../../Scripting/LuaController.h"
#include "../../System/Log.h"

namespace BlackBoxEngine
{
    void LuaScriptComponent::LoadScript(const char* filePath)
    {
        m_pFilePath = filePath;
        m_pLuaScript = ResourceManager::GetLuaScript(filePath);
    }

    void LuaScriptComponent::RunScript()
    {
        LuaController::Get()->RunLuaScript(m_pLuaScript);
    }

    void LuaScriptComponent::Load(const XMLElementParser parser)
    {
        parser.GetChildVariable("filePath", &m_pFilePath);

        if (!m_pFilePath)
            BB_LOG(LogType::kFailure, "Bad filePath");
        LoadScript(m_pFilePath);
    }

    void LuaScriptComponent::Save(XMLElementParser parser)
    {
        parser.NewChildVariable("filePath", m_pFilePath);
    }
}
