#include "XMLFactory.h"

#include "../../System/StringHash.h"
#include "../../System/Log.h"

#include "tinyxml2.h"

namespace BlackBoxEngine
{
    tinyxml2::XMLDocument* XMLFactory::CreateNewXMLFile(HashType hash, const char* pFilePath)
    {
        // create the new xmlFile
        tinyxml2::XMLDocument* pDocument = new tinyxml2::XMLDocument;
        pDocument->LoadFile(pFilePath);
        if (pDocument->Error())
            BB_LOG(LogType::kError, "Error with document load : ", pDocument->ErrorName());
        auto emplacePair = m_XMLDocMap.try_emplace(hash, pDocument);

        // log errors
        if (!pDocument)
            BB_LOG(LogType::kError, "Error importing xmlFile : \"", pFilePath, '\"');
        else
            BB_LOG(LogType::kMessage, pFilePath, " imported properly");

        return pDocument;
    }

    LevelXMLParser XMLFactory::CreateLevelFromFile(const char* pFilePath)
    {
        HashType hash = StringHash(pFilePath);

        // find if already in cache
        auto it = m_XMLDocMap.find(hash);
        if (it != m_XMLDocMap.end())
        {
            return LevelXMLParser(it->second);
        }

        return LevelXMLParser(CreateNewXMLFile(hash, pFilePath));
    }

    ActorXMLParser XMLFactory::CreateActorFromFile(const char* pFilePath)
    {
        HashType hash = StringHash(pFilePath);

        //// find if already in cache
        auto it = m_XMLDocMap.find(hash);
        if (it != m_XMLDocMap.end())
        {
            return ActorXMLParser(it->second);
        }

        return ActorXMLParser(CreateNewXMLFile(hash, pFilePath));
    }

    XMLElementParser XMLFactory::CreateXMLFile(const char* pFilePath)
    {
        HashType hash = StringHash(pFilePath);

        // find if already in cache
        auto it = m_XMLDocMap.find(hash);
        if (it != m_XMLDocMap.end())
        {
            return XMLElementParser(it->second->RootElement());
        }

        auto* pDoc = CreateNewXMLFile(hash, pFilePath);
        return XMLElementParser(pDoc->RootElement());
    }

    void XMLFactory::ClearCache()
    {
        for (auto& [id, ptr] : m_XMLDocMap)
            delete ptr;
        m_XMLDocMap.clear();
    }
    void XMLFactory::RemoveFromCache( const char* pFilePath )
    {
        auto id = StringHash( pFilePath );
        auto it = m_XMLDocMap.find( id );
        if ( it == m_XMLDocMap.end() )
            return;
        m_XMLDocMap.erase( it );
    }
}


