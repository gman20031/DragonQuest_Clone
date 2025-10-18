#include "ComponentFactory.h"

#include <tuple>

#include "EngineComponents/SpriteComponent.h"
#include "EngineComponents/TransformComponent.h"
#include "EngineComponents/MoverComponent.h"
#include "EngineComponents/MovementBlocker.h"
#include "EngineComponents/ColliderDebugRenderer.h"
#include "EngineComponents/LuaScriptComponent.h"
#include "EngineComponents/SimpleTextComponent.h"
#include "EngineComponents/PlayerCameraComponent.h"
#include "Collision/AABBColliderComponent.h"
#include "../Audio/AudioPlayerComponent.h"

namespace BlackBoxEngine
{
    static std::tuple
    <
        ComponentFactory::Registrar<AABBColliderComponent>,
        ComponentFactory::Registrar<TransformComponent>,
        ComponentFactory::Registrar<AnimatedSpriteComponent>,
        ComponentFactory::Registrar<SpriteComponent>,
        ComponentFactory::Registrar<MoverComponent>,
        ComponentFactory::Registrar<ColliderDebugRenderer>,
        ComponentFactory::Registrar<MovementBlocker>,
        ComponentFactory::Registrar<LuaScriptComponent>,
        ComponentFactory::Registrar<SimpleTextComponent>,
        ComponentFactory::Registrar<PlayerCameraComponent>,
        ComponentFactory::Registrar<AudioPlayerComponent>,
        ComponentFactory::Registrar<MusicPlayer>
    > EngineRegistrar;

    Component* ComponentFactory::NewComponent(Component::Id id, Actor* pOwner)
    {
        auto it = Map().find(id);
        if (it == Map().end())
        {
            BB_LOG(LogType::kFailure, "Attempted to create component that was not registered ID:", id);
            return nullptr;
        }
        return it->second(pOwner);
    }
}

