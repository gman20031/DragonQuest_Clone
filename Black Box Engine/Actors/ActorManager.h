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
        std::unordered_map<Actor::Id , ActorPtr> m_allActors;
        std::vector<Actor::Id> m_unusedIds;
        std::vector<Actor::Id> m_destroyQueue;
        std::vector<Actor::Id> m_startQueue;
        std::recursive_mutex m_actorMutex;

        Actor::Id m_highestId = 0;

    protected:
        Actor::Id NextId();
        void RemoveQueuedActors();
    public:
        const ActorPtr& NewActor();
        const ActorPtr& LoadActor(const char* filePath);
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

};

