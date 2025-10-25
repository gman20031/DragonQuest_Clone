#pragma once

#include <memory>
#include <unordered_map>
#include "stdint.h"

#include "Component.h"
#include "../Resources/XML/BlackBoxXMLParser.h"

namespace BlackBoxEngine
{
    class Actor
	{
        friend class ActorXMLParser;
        friend class ActorManager;
        inline static constexpr bool kLogSuccess = false;
	public:
		friend bool operator==(const Actor& lhs, const Actor& rhs);
		using Id = uint32_t;

	private:
		ActorManager* m_pActorManager = nullptr;
		std::unordered_map<Id, Component*> m_componentMap;
        Id m_id;

	public:
		Actor(ActorManager* pManager, Id id);
        Actor() = delete;
        ~Actor();
        Actor(const Actor&) = delete;
        Actor(Actor&&) = default;
        Actor& operator=(const Actor&) = delete;
        Actor& operator=(Actor&&) = default;

        Id GetId() const { return m_id; }
        ActorManager* Manager() const { return m_pActorManager; }

        bool ParseComponent(const XMLElementParser componentParser);
		template<SubComponent ComponentType, typename ...Args> ComponentType* AddComponent(Args&&... args);
        Component* AddComponent(Component::Id componentId);
		[[nodiscard("Component Unused")]] Component* GetComponent(uint32_t componentId);
        template<SubComponent ComponentType> [[nodiscard("Component Unused")]] ComponentType* GetComponent();

		void Update();
		void Render();
        void Start();
        void Collided(Actor* pOther);
	};

	template<SubComponent ComponentType , typename ...Args>
	inline ComponentType* Actor::AddComponent(Args&&... args)
	{
        if (m_componentMap.contains(ComponentType::s_id))
            return nullptr;

        ComponentType* pOut = new ComponentType(this, std::forward<Args>(args)...);
		m_componentMap.try_emplace(ComponentType::s_id, pOut);
		return pOut;
	}

	template<SubComponent ComponentType>
	inline ComponentType* Actor::GetComponent()
    {
        return static_cast<ComponentType*> (GetComponent(ComponentType::s_id));
    }

    template<SubComponent ComponentType>
    ComponentType* FindOrCreateComponent(Actor* pActor)
    {
        ComponentType* pComponent = pActor->GetComponent<ComponentType>();
        if (!pComponent)
            pComponent = pActor->AddComponent<ComponentType>();
        return pComponent;
    }

}// BlackBoxEngine

