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

        XMLFactory() = default;

        ActorXMLParser CreateActorFromFile(const char* pFilePath);
        LevelXMLParser CreateLevel(const char* pFilePath);
       
        void ClearCache();
    };

}