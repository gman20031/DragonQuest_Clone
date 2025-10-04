#pragma once

#include <unordered_map>
#include <filesystem>

#include "Actor.h"

namespace BlackBoxEngine
{
    class ActorManager
    {
    public:
        using ActorPtr = std::unique_ptr<Actor>;
    protected:
        std::unordered_map<Actor::Id , ActorPtr> m_allActors;
        std::vector<Actor::Id> m_unused;
        std::vector<Actor::Id> m_destroyQueue;

        Actor::Id m_highestId = 0;

    protected:
        Actor::Id NextId();
        void RemoveQueuedActors();
    public:
        const ActorPtr& NewActor();
        const ActorPtr& LoadActor(const char* filePath);
        void LoadLevel(const char* filePath);
        void ClearLevel();
        void DestroyActor(Actor::Id id);
        void DestroyActor(Actor* pActor);
        
        const ActorPtr& GetActor(Actor::Id id);

        void Update();
        void Start(); // where the hell should I call this?
        void Render();
        
        static void SaveActor( const std::unique_ptr<Actor>& pActor, const char* pActorName, std::filesystem::path filePath = {} );
        static void SaveActor(const Actor* pActor, const char* pActorName, std::filesystem::path filePath = {});
    };

};

