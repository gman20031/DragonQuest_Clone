#include "EncounterHandler.h"

#include <cassert>
#include <BlackBoxManager.h>
#include <Resources/ResourceManager.h>


using namespace BlackBoxEngine;

EncounterHandler::EncounterHandler()
{
    m_randomMachine.Reset( (uint64_t)std::time( 0 ) );
    LoadEncounterData();
}

const EncounterHandler::ActorPtr& EncounterHandler::GetEncounterActorAtTile( int x, int y )
{
    int index = x + (y * (m_encounterZoneSize * m_encounterCountX));
    assert( index > 0 && index < m_encounterZones.size() * m_encounterZoneSize * m_encounterZoneSize );
    float zoneIndex = float( index ) / (m_encounterZoneSize * m_encounterZoneSize);
    index = static_cast<int>( floorf( zoneIndex ) );
    int encounterId = m_encounterZones[index];
    auto& encounterVector = m_encounterMap[encounterId];
    int encounterIndex = m_randomMachine.GetRandomInRange( 0, (int)encounterVector.size() - 1 );
    ActorXMLParser xml = encounterVector[encounterIndex];
    return BlackBoxManager::Get()->m_pActorManager->MakeActor( xml );
}

void EncounterHandler::LoadEncounterData()
{
    auto element = ResourceManager::GetRawXMLDATA( "../Assets/Actors/Encounters/EncounterInfo.xml" );
    element.GetChildVariable( "EncounterAreaStartX", &m_encounterTileStartX );
    element.GetChildVariable( "EncounterAreaStartY", &m_encounterTileStartY );
    element.GetChildVariable( "EncounterZoneSize", &m_encounterZoneSize );
    element.GetChildVariable( "EncounterWidth", &m_encounterCountX );

    auto array = element.GetChildElement( "EncounterGrid" );
    if ( !array )
        BB_LOG( LogType::kError, "EncounterGrid not found when loading encouner handler data" );

    auto arrayNode = array.GetChildElement( "element" );
    int arrayElement = 0;
    while( arrayNode )
    {
        arrayNode.GetInt(&arrayElement);
        m_encounterZones.emplace_back( arrayNode );
        arrayNode = arrayNode.GetSibling();
    }

    auto encounterMap = element.GetChildElement( "EncounterMap" );
    if ( !encounterMap )
        BB_LOG(LogType::kError , "EncounterMap not found when loading encouner handler data");

    int encounterId = 0;
    auto mapElement = encounterMap.GetChildElement( "encounterKey" );
    while ( mapElement )
    {
        auto encounter = mapElement.GetChildElement( "encounterInfo" );
        while ( encounter )
        {
            const char* pText = nullptr;
            encounter.GetText( &pText );
            auto encounterXML = ResourceManager::GetActorXMLData( pText );
            m_encounterMap[encounterId].emplace_back( encounterXML );
            encounter = encounter.GetSibling();
        }
        ++encounterId;
        mapElement = mapElement.GetSibling();
    }
}
