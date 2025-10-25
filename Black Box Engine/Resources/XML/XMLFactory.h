#pragma once

#include <memory>
#include <unordered_map>

#include "BlackBoxXMLParser.h"

class tinyxml2::XMLDocument;

namespace BlackBoxEngine
{
    class XMLFactory
    {
        friend class ResourceManager;

        using HashType = uint32_t;
        std::unordered_map<HashType, tinyxml2::XMLDocument* > m_XMLDocMap;
        
        tinyxml2::XMLDocument* CreateNewXMLFile(HashType hash, const char* pFilePath);
        ActorXMLParser CreateActorFromFile(const char* pFilePath);
        LevelXMLParser CreateLevelFromFile(const char* pFilePath);
        XMLElementParser CreateXMLFile(const char* pFilePath);

        void ClearCache();
        void RemoveFromCache(const char* pFilePath);
    };

}