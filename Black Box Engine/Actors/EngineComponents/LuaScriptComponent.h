#pragma once

#include "../Component.h"
#include "../../Resources/ResourceManager.h"

namespace BlackBoxEngine
{
    class LuaScriptComponent : public Component
    {
    public:
        GENERATE_ID("LuaScriptComponent");
    private:
        const char* m_pFilePath = nullptr;
        std::shared_ptr<LuaScript> m_pLuaScript;

    public:
        LuaScriptComponent(Actor* pOwner) : Component(pOwner) { /*Empty*/ };
        virtual ~LuaScriptComponent() = default;

        void LoadScript(const char* filePath);
        void RunScript();

        virtual void Load(const XMLElementParser parser) override;
        virtual void Save(XMLElementParser parser) override;
    };

}