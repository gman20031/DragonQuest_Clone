#include "Actor.h"

#include <algorithm>

#include "ComponentFactory.h"
#include "../System/Log.h"
#include "../Resources/XML/tinyxml2.h"

namespace BlackBoxEngine
{
    Actor::Actor(ActorManager* pManager, Actor::Id id)
        : m_pActorManager(pManager)
        , m_id(id)
    {
    }

    Actor::~Actor()
    {
        for (auto& [id, pActor] : m_componentMap)
            delete pActor;
    }

    Component* Actor::AddComponent(Component::Id componentId)
    {
        if (m_componentMap.contains(componentId))
            return nullptr;

        Component* pOut = ComponentFactory::NewComponent(componentId, this);
        m_componentMap.emplace(componentId, pOut);
        return pOut;
    }

    bool Actor::ParseComponent(const XMLElementParser componentParser)
    {
        if( ! componentParser )
            return false;

        if constexpr (kLogSuccess)
            BB_LOG(LogType::kMessage, "Adding component : " , componentParser.GetFirstAttribute() );
        Component* pComponent = AddComponent( StringHash(componentParser.GetFirstAttribute() ) );

        if (!pComponent)
        {
            BB_LOG( LogType::kError, "Component creation failed, ", componentParser.GetFirstAttribute() );
            return false;
        }
        
        pComponent->Load(componentParser);
        return true;
    }

    Component* Actor::GetComponent(uint32_t componentId)
    {
        auto it = m_componentMap.find(componentId);
        if( it == m_componentMap.end() )
            return nullptr; // returning nullptr is a valid response. Not error/warning worthy
        return it->second;
    }

    void Actor::Update()
    {
        for (const auto& [id, pComponent] : m_componentMap)
            pComponent->Update();
    }

    void Actor::Render()
    {
        for (const auto& [id, pComponent] : m_componentMap)
            pComponent->Render();
    }

    void Actor::Start()
    {
        for (const auto& [id, pComponent] : m_componentMap)
            pComponent->Start();
    }

    void Actor::Collided(Actor* pOther)
    {
        for (const auto& [id, pComponent] : m_componentMap)
            pComponent->OnCollide(pOther);
    }

    bool operator==(const Actor& lhs, const Actor& rhs)
    {
        return (lhs.m_id == rhs.m_id);
    }
}
