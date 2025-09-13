#pragma once

#include <memory>
#include <unordered_map>

#include "BlackBoxXMLParser.h"

namespace BlackBoxEngine
{
    class XMLFactory
    {
        friend class ResourceManager;

        using HashType = uint32_t;
        std::unordered_map<HashType, tinyxml2::XMLDocument* > m_XMLDocMap;

        ActorXMLParser CreateActorFromFile(const char* pFilePath);
        LevelXMLParser CreateLevelFromFile(const char* pFilePath);
       
        void ClearCache();
    };

}