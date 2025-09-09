#include "ActorManager.h"

#include "../System/Log.h"
#include "../Resources/ResourceManager.h"
#include "../Resources/XML/BlackBoxXMLParser.h"
#include "../Resources/XML/tinyxml2.h"

namespace BlackBoxEngine
{
    void ActorManager::Update()
    {
        RemoveQueuedActors();
        for (auto& [id, pActor] : m_allActors)
            pActor->Update();
    }

    void ActorManager::Start()
    {
        for (auto& [id, pActor] : m_allActors)
            pActor->Start();
    }

    void ActorManager::Render()
    {
        for (auto& [id, pActor] : m_allActors)
            pActor->Render();
    }

    const ActorManager::ActorPtr& ActorManager::NewActor()
    {
        Actor::Id id = NextId();
        auto [pair, test] = m_allActors.emplace(id, std::make_unique<Actor>(this, id) );
        if (!test)
            BB_LOG(LogType::kError, "Actor emplace failed");
        return pair->second;
    }

    const ActorManager::ActorPtr& ActorManager::LoadActor(const char* filePath)
    {
        auto parser = ResourceManager::GetActorXMLData(filePath);
        auto& pActor = NewActor();
        while ( pActor->ParseComponent(parser.NextComponent()) );
        return pActor;
    }

    void ActorManager::LoadLevel(const char* filePath)
    {
        LevelXMLParser LevelParser = ResourceManager::GetLevelXMLData(filePath);
        ActorXMLParser actorParser;

        while (LevelParser.HasActors())
        {
            actorParser = LevelParser.GetNextActor();
            auto& pActor = NewActor();
            while (pActor->ParseComponent(actorParser.NextComponent()));
        }
    }

    Actor::Id ActorManager::NextId()
    {
        if (m_unsuedIds.empty())
        {
            ++m_highestId;
            return m_highestId;
            
        }
        Actor::Id nextId = m_unsuedIds.back();
        m_unsuedIds.pop_back();
        return nextId;
    }

    void ActorManager::RemoveQueuedActors()
    {
        for (auto id : m_destroyQueue)
        {
            m_allActors.erase(id);
            m_unsuedIds.push_back(id);
        }
    }

    void ActorManager::DestroyActor(Actor::Id id)
    {
        m_destroyQueue.emplace_back(id);
    }

    void ActorManager::DestroyActor(Actor* pActor)
    {
        DestroyActor(pActor->GetId());
    }

}