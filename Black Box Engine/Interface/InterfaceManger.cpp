#include "InterfaceManger.h"

namespace BlackBoxEngine
{
    int32_t InterfaceManager::GetID()
    {
        if (m_unusedIds.empty())
        {
            auto id = m_nextID;
            ++m_nextID;
            return id;
        }
        int32_t nextId = m_unusedIds.back();
        m_unusedIds.pop_back();
        return nextId;
    }

    int32_t InterfaceManager::AddInterfaceToScreen(UserInterface* pInterface)
    {
        int32_t id = GetID();
        m_interfaces.emplace(id, pInterface);
        return id;
    }

    void InterfaceManager::RemoveInterface(int32_t id)
    {
        auto it = m_interfaces.find(id);
        if (it != m_interfaces.end())
            m_interfaces.erase(it);
    }

    void InterfaceManager::Update()
    {
        for (auto& [key,interface]: m_interfaces)
            interface->Update();
    }

    void InterfaceManager::Render(BB_Renderer* pRenderer)
    {
        for (auto& [key, interface] : m_interfaces)
            interface->Render(pRenderer);
    }
}