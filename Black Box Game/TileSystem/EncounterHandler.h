#pragma once

#include <vector>
#include <unordered_map>
#include <Resources/XML/BlackBoxXMLParser.h>
#include <Math/Random.h>

class EncounterHandler
{
    using ActorPtr = std::unique_ptr<BlackBoxEngine::Actor>;
    BlackBoxEngine::Random::MachineXoshiro256 m_randomMachine;
    std::vector<int> m_encounterZones;
    std::unordered_map< int, std::vector<BlackBoxEngine::ActorXMLParser>> m_encounterMap;
    int m_encounterTileStartX = 0;
    int m_encounterTileStartY = 0;
    int m_encounterZoneSize = 0;
    int m_encounterCountX = 0;

public:
    EncounterHandler();

    const ActorPtr& GetEncounterActorAtTile( int x, int y );
private:
    void LoadEncounterData();
};
