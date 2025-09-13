#include "XMLFactory.h"

#include "../../System/StringHash.h"
#include "../../System/Log.h"

#include "tinyxml2.h"

namespace BlackBoxEngine
{
    //XMLFactory::~XMLFactory()
    //{
    //    ClearCache();
    //}

    LevelXMLParser XMLFactory::CreateLevelFromFile(const char* pFilePath)
    {
        HashType hash = StringHash(pFilePath);

        // find if already in cache
        auto it = m_XMLDocMap.find(hash);
        if (it != m_XMLDocMap.end())
        {
            return LevelXMLParser(it->second);
        }

        // create the new xmlFile
        tinyxml2::XMLDocument* pDocument = new tinyxml2::XMLDocument;
        pDocument->LoadFile(pFilePath);
        if (pDocument->Error())
            BB_LOG(LogType::kError, "Error with document load : ", pDocument->ErrorName());

        auto emplacePair = m_XMLDocMap.emplace(hash, pDocument);

        // log errors
        if (!emplacePair.second || !pDocument)
            BB_LOG(LogType::kError, "Error importing xmlFile : \"", pFilePath, '\"');
        else
            BB_LOG(LogType::kMessage, pFilePath, " imported properly");

        return LevelXMLParser(pDocument);
    }

    ActorXMLParser XMLFactory::CreateActorFromFile(const char* pFilePath)
    {
        HashType hash = StringHash(pFilePath);

        // find if already in cache
        auto it = m_XMLDocMap.find(hash);
        if (it != m_XMLDocMap.end())
        {
            return ActorXMLParser(it->second);
        }

        // create the new xmlFile
        tinyxml2::XMLDocument* pDocument = new tinyxml2::XMLDocument;
        pDocument->LoadFile(pFilePath);
        if (pDocument->Error())
            BB_LOG(LogType::kError, "Error with document load : ", pDocument->ErrorName());
        auto emplacePair = m_XMLDocMap.emplace(hash, pDocument);

        // log errors
        if (!emplacePair.second || !pDocument)
            BB_LOG(LogType::kError, "Error importing xmlFile : \"", pFilePath, '\"');
        else
            BB_LOG(LogType::kMessage, pFilePath, " imported properly");

        return ActorXMLParser(pDocument);
    }

    void XMLFactory::ClearCache()
    {
        for (auto& [id, ptr] : m_XMLDocMap)
        {
            if (ptr)
                delete ptr;
        }
        m_XMLDocMap.clear();
    }
}


