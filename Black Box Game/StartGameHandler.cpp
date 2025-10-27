#include "StartGameHandler.h"

#include <BlackBoxManager.h>
#include <Actors/Actor.h>
#include <Actors/EngineComponents/TransformComponent.h>
#include <Resources/ResourceManager.h>

#include "PlayerStatsComponent.h"
#include "InventoryComponent.h"

using namespace BlackBoxEngine;

GameStarter::GameStarter( const char* pPlayerInfoXML )
    : m_pPlayerInfoXML(pPlayerInfoXML)
    , m_playerStatParser( ResourceManager::GetRawXMLDATA( m_pPlayerInfoXML ) )
{
    m_playerStatParser.GetChildVariable( "PlayerLevel", &m_level );
    m_playerStatParser.GetChildVariable( "PlayerExp", &m_exp );
    m_playerStatParser.GetChildVariable( "PlayerHP", &m_hp );
    m_playerStatParser.GetChildVariable( "PlayerMaxHP", &m_maxHP );
    m_playerStatParser.GetChildVariable( "PlayerMP", &m_mp );
    m_playerStatParser.GetChildVariable( "PlayerMaxMP", &m_maxMP );
    m_playerStatParser.GetChildVariable( "PlayerGold", &m_gold );
    m_playerStatParser.GetChildVariable( "PlayerAgility", &m_agility );
    m_playerStatParser.GetChildVariable( "PlayerStrength", &m_strength );
    auto element = m_playerStatParser.GetChildElement( "PlayerStartPosition" );
    if ( element )
    {
        element.GetChildVariable( "Xpos", &m_x );
        element.GetChildVariable( "Ypos", &m_y );
    }
    else
    {
        BB_LOG( LogType::kError, "Player starting information xml does not contain playerStart" );
    }
}

void GameStarter::SetPlayerStats( BlackBoxEngine::Actor* pPlayer ) const
{
    auto* pStats = pPlayer->GetComponent<PlayerStatsComponent>();
    if ( !pStats )
    {
        BB_LOG( LogType::kWarning, "Attempted to set player stat on actor without player stat component" );
        return;
    }
    pStats->SetPlayerLevel( m_level );
    pStats->SetPlayerHP( m_hp );
    pStats->SetPlayerMaxHP( m_maxHP );
    pStats->SetPlayerMP( m_mp );
    pStats->SetPlayerMaxMP( m_maxMP );
    pStats->SetPlayerGold( m_gold);
    pStats->SetPlayerStrength( m_strength );
    pStats->SetPlayerAgility( m_agility );
    pStats->SetPlayerExperience( m_exp );
}

void GameStarter::SetPlayerPosition( BlackBoxEngine::Actor* pPlayer ) const
{
    auto* pTransform = pPlayer->GetComponent<TransformComponent>();
    if ( !pTransform )
    {
        BB_LOG( LogType::kWarning, "Attempted to set player pos on actor without transform" );
        return;
    }
    pTransform->m_position = {m_x, m_y};
    pTransform->m_prevPosition = {m_x, m_y};
}

void GameStarter::StartGamePlayer( BlackBoxEngine::Actor* pPlayer ) const
{
    SetPlayerStats( pPlayer );
    SetPlayerPosition( pPlayer );
}

void GameStarter::Start() const
{
    auto& pActor = BlackBoxManager::Get()->m_pActorManager->FindActorWithComponent<PlayerStatsComponent>();
    StartGamePlayer( pActor.get() );
    pActor->GetComponent<InventoryComponent>()->SetHasTablet( false );
}