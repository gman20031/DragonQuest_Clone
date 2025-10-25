#pragma once

#include <unordered_map>
#include <filesystem>
#include <mutex>

#include "Actor.h"

namespace BlackBoxEngine
{
    class ActorManager
    {
    public:
        using ActorPtr = std::unique_ptr<Actor>;
    protected:
        std::unordered_map<Actor::Id, ActorPtr> m_activeActors;
        std::unordered_map<Actor::Id, ActorPtr> m_newActors;
        std::unordered_map<Actor::Id, ActorPtr> m_deletedActors;

        std::vector<Actor::Id> m_unusedIds;
        std::recursive_mutex m_actorMutex;

        Actor::Id m_highestId = 0;

    protected:
        Actor::Id NextId();
        void RemoveQueuedActors();
        void MakeNewActorsActive();

    public:
        const ActorPtr& NewActor();
        const ActorPtr& MakeActor(ActorXMLParser actorParser );
        const ActorPtr& LoadActor( const char* filePath );
        template<SubComponent ComponenentType>
        const ActorPtr& FindActorWithComponent();
        void LoadLevel(const char* filePath);
        void DestroyActor(Actor::Id id);
        void DestroyActor(Actor* pActor);
        void DestoryAllActors();
        
        const ActorPtr& GetActor(Actor::Id id);

        void Update();
        void Render();
        
        static void SaveActor( const std::unique_ptr<Actor>& pActor, const char* pActorName, std::filesystem::path filePath = {} );
        static void SaveActor(const Actor* pActor, const char* pActorName, std::filesystem::path filePath = {});
    };

    template<SubComponent ComponenentType>
    inline const ActorManager::ActorPtr& ActorManager::FindActorWithComponent( )
    {
        for ( const auto& [id, pActor] : m_activeActors )
        {
            auto* pComponent = pActor->GetComponent<ComponenentType>();
            if ( pComponent )
                return pActor;
        }
        for ( const auto& [id, pActor] : m_newActors )
        {
            auto* pComponent = pActor->GetComponent<ComponenentType>();
            if ( pComponent )
                return pActor;
        }
        return GetActor((uint32_t) - 1);
    }
};
