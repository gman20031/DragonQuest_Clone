#include "ActorManager.h"

#include "../System/Log.h"
#include "../Resources/ResourceManager.h"
#include "../Resources/XML/BlackBoxXMLParser.h"
#include "../Resources/XML/tinyxml2.h"
#include "../BlackBoxManager.h"

namespace BlackBoxEngine
{
    void ActorManager::Update()
    {
        std::unique_lock lock( m_actorMutex );
        RemoveQueuedActors();
        for (auto& [id, pActor] : m_allActors)
            pActor->Update();
    }

    void ActorManager::Start()
    {
        std::unique_lock lock( m_actorMutex );
        for (auto& [id, pActor] : m_allActors)
            pActor->Start();
    }

    void ActorManager::Render()
    {
        std::unique_lock lock( m_actorMutex );
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
        std::unique_lock lock( m_actorMutex );
        InternalClearLevel();

        LevelXMLParser LevelParser = ResourceManager::GetLevelXMLData(filePath);
        ActorXMLParser actorParser;

        while (LevelParser.HasActors())
        {
            actorParser = LevelParser.GetNextActor();
            auto& pActor = NewActor();
            while (pActor->ParseComponent(actorParser.NextComponent()));
        }
    }

    void ActorManager::SaveActor(const std::unique_ptr<Actor>& pActor,
                                const char* pActorName,
                                std::filesystem::path filePath)
    {
        ActorXMLParser::SaveActor(pActor, pActorName, filePath);
    }

    void ActorManager::SaveActor( const Actor* pActor,
        const char* pActorName,
        std::filesystem::path filePath )
    {
        ActorXMLParser::SaveActor( pActor, pActorName, filePath );
    }

    const ActorManager::ActorPtr& ActorManager::GetActor(Actor::Id id)
    {
        std::unique_lock lock( m_actorMutex );
        auto it = m_allActors.find(id);
        if (it != m_allActors.end() )
            return it->second;

        BB_LOG(LogType::kError, "Attempted to get actor that does not exist. Making a temp empty actor, ID: " , id);
        const auto& ret = NewActor();
        DestroyActor(ret->GetId());
        return ret;
    }

    void ActorManager::DestroyActor( Actor::Id id )
    {
        std::unique_lock lock( m_actorMutex );
        m_destroyQueue.emplace_back( id );
    }

    void ActorManager::DestroyActor( Actor* pActor )
    {
        std::unique_lock lock( m_actorMutex );
        DestroyActor( pActor->GetId() );
    }

    void ActorManager::DestoryAllActors()
    {
        for ( auto& [id, actor] : m_allActors )
            DestroyActor( id );
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

    void ActorManager::InternalClearLevel()
    {
        m_allActors.clear();
        m_unused.clear();
        m_highestId = 0;
    }
}