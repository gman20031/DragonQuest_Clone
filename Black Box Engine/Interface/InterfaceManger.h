#pragma once

#include "UserInterface.h"

namespace BlackBoxEngine
{
    class InterfaceManager
    {
        std::unordered_map<int32_t , UserInterface*> m_interfaces;
        std::vector<int32_t> m_unusedIds;

        int32_t m_nextID = 1;

    private:
        int32_t GetID();
    public:

        [[nodiscard]]int32_t AddInterfaceToScreen(UserInterface* pInterface);
        void RemoveInterface(int32_t id);

        void Update();
        void Render(BB_Renderer* pRenderer);
    };
}
