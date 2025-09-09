#pragma once

#include <unordered_map>

#include "../Actors/Component.h"
#include "../System/Log.h"

namespace BlackBoxEngine
{

    class ComponentFactory
    {
        using FactoryFunction = Component* (*) (Actor*);
        using ComponentMap = std::unordered_map <Component::Id, FactoryFunction>;

        static ComponentMap& Map()
        {
            static ComponentMap s_componentMap;
            return s_componentMap;
        }
    public:
        ComponentFactory() = delete;

        static Component* NewComponent(Component::Id id , Actor* pActor);

        template<SubComponent ComponentType>
        class Registrar
        {
        private:
            static Component* ComponentMaker(Actor* pActor) { return new ComponentType(pActor); }
        public:
            Registrar();
        };
    };

    template<SubComponent ComponentType>
    inline ComponentFactory::Registrar<ComponentType>::Registrar()
    {
        const auto& [pair , good] = Map().try_emplace(ComponentType::s_id, &ComponentMaker);
        if ( good )
            BB_LOG(LogType::kMessage, "Component Registered: ", ComponentType::s_name);
        else
            BB_LOG(LogType::kWarning, "Component Failed to register, repeated ID: ", ComponentType::s_name);
    }
}


