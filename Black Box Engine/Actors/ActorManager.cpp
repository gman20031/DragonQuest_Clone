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

    void ActorManager::ClearLevel()
    {
        m_allActors.clear();
        m_unused.clear();
        m_highestId = 0;
    }

    void ActorManager::LoadLevel(const char* filePath)
    {
        ClearLevel();

        LevelXMLParser LevelParser = ResourceManager::GetLevelXMLData(filePath);
        ActorXMLParser actorParser;

        while (LevelParser.HasActors())
        {
            actorParser = LevelParser.GetNextActor();
            auto& pActor = NewActor();
            while (pActor->ParseComponent(actorParser.NextComponent()));
        }
        
        Start();
    }

    void ActorManager::SaveActor(const std::unique_ptr<Actor>& pActor,
                                const char* pActorName,
                                std::filesystem::path filePath)
    {
        ActorXMLParser::SaveActor(pActor, pActorName, filePath);
    }

    const ActorManager::ActorPtr& ActorManager::GetActor(Actor::Id id)
    {
        auto it = m_allActors.find(id);
        if (it != m_allActors.end() )
            return it->second;

        BB_LOG(LogType::kError, "Attempted to get actor that does not exist. Making a temp empty actor, ID: " , id);
        const auto& ret = NewActor();
        DestroyActor(ret->GetId());
        return ret;
    }

    Actor::Id ActorManager::NextId()
    {
        if (m_unused.empty())
        {
            auto id = m_highestId;
            ++m_highestId;
            return id;
        }
        Actor::Id nextId = m_unused.back();
        m_unused.pop_back();
        return nextId;
    }

    void ActorManager::RemoveQueuedActors()
    {
        for (auto id : m_destroyQueue)
        {
            m_allActors.erase(id);
            m_unused.push_back(id);
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