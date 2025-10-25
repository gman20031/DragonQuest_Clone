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
        MakeNewActorsActive();
        for (auto& [id, pActor] : m_activeActors)
            pActor->Update();
    }

    void ActorManager::Render()
    {
        std::unique_lock lock( m_actorMutex );
        for (auto& [id, pActor] : m_activeActors)
            pActor->Render();
    }

    const ActorManager::ActorPtr& ActorManager::NewActor()
    {
        std::unique_lock lock( m_actorMutex );
        Actor::Id id = NextId();
        auto [pair, test] = m_newActors.emplace(id, std::make_unique<Actor>(this, id) );
        if (!test)
            BB_LOG(LogType::kError, "Actor emplace failed");
        return pair->second;
    }

    const ActorManager::ActorPtr& ActorManager::MakeActor( ActorXMLParser actorParser )
    {
        auto& pActor = NewActor();
        while ( pActor->ParseComponent( actorParser.NextComponent() ) );
        return pActor;
    }

    const ActorManager::ActorPtr& ActorManager::LoadActor( const char* filePath )
    {
        auto parser = ResourceManager::GetActorXMLData( filePath );
        auto& pActor = NewActor();
        while ( pActor->ParseComponent( parser.NextComponent() ) );
        return pActor;
    }

    void ActorManager::LoadLevel(const char* filePath)
    {
        DestoryAllActors();

        LevelXMLParser LevelParser = ResourceManager::GetLevelXMLData(filePath);
        ActorXMLParser actorParser;

        std::unique_lock lock( m_actorMutex );
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
        auto it = m_activeActors.find(id);
        if (it != m_activeActors.end() )
            return it->second;

        it = m_newActors.find( id );
        if ( it != m_newActors.end() )
            return it->second;

        BB_LOG(LogType::kError, "Attempted to get actor that does not exist. Making a temp empty actor, ID: " , id);
        const auto& ret = NewActor();
        DestroyActor(ret->GetId());
        return ret;
    }

    void ActorManager::DestroyActor( Actor::Id id )
    {
        std::unique_lock lock( m_actorMutex );
        auto it = m_activeActors.find( id );
        if ( it == m_activeActors.end() )
            return;
        
        m_deletedActors.emplace( id, std::move( it->second ) );
        m_activeActors.erase( it );
        m_unusedIds.emplace_back( id );
    }

    void ActorManager::DestroyActor( Actor* pActor )
    {
        std::unique_lock lock( m_actorMutex );
        DestroyActor( pActor->GetId() );
    }

    void ActorManager::DestoryAllActors()
    {
        std::unique_lock lock( m_actorMutex );
        while ( !m_activeActors.empty() )
            DestroyActor( m_activeActors.begin()->first );
        m_unusedIds.clear();
        m_highestId = 0;
    }

    ////////////////////////////////////////////////////////////////////////////// 
    /// private
    //////////////////////////////////////////////////////////////////////////////

    Actor::Id ActorManager::NextId()
    {
        if (m_unusedIds.empty())
        {
            auto id = m_highestId;
            ++m_highestId;
            return id;
        }
        Actor::Id nextId = m_unusedIds.back();
        m_unusedIds.pop_back();
        return nextId;
    }

    void ActorManager::RemoveQueuedActors()
    {
        m_deletedActors.clear();
    }

    void ActorManager::MakeNewActorsActive()
    {
        for ( auto it = m_newActors.begin(); it != m_newActors.end(); ++it )
        {
            ActorPtr pActor = std::move( it->second );
            pActor->Start();
            const auto& pair = m_activeActors.emplace( it->first, std::move( pActor ) );
            if ( !pair.second && pActor )
            {
                BB_LOG( LogType::kError, "Transfer of new actors failed, new actor id " , pActor->GetId() 
                , ", ret actor id " , pair.first->second->GetId() );
            }
        }
        m_newActors.clear();
    }

}