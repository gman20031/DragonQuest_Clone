#pragma once

#include "Actor.h"
#include <unordered_map>

namespace BlackBoxEngine
{
    class ActorManager
    {
    public:
        using ActorPtr = std::unique_ptr<Actor>;
    private:
        std::unordered_map<Actor::Id , ActorPtr> m_allActors;
        std::vector<Actor::Id> m_unsuedIds;
        std::vector<Actor::Id> m_destroyQueue;

        Actor::Id m_highestId = 0;

    private:
        Actor::Id NextId();
        void RemoveQueuedActors();
    public:

        const ActorPtr& NewActor();
        const ActorPtr& LoadActor(const char* filePath);
        void LoadLevel(const char* filePath);
        void DestroyActor(Actor::Id id);
        void DestroyActor(Actor* pActor);
        
        void Update();
        void Start(); // where the hell should I call this?
        void Render();
    };

};

