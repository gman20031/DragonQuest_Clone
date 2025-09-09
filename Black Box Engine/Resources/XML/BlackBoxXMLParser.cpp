#include "BlackBoxXMLParser.h"

#include "tinyxml2.h"
#include "../ResourceManager.h"
#include "../../System/Log.h"
#include "../../Actors/Actor.h"

namespace BlackBoxEngine
{

    //------------------------------------------------------------------------------------------------------------
    // Level Xml Parser
    //------------------------------------------------------------------------------------------------------------
    
    tinyxml2::XMLElement* LevelXMLParser::HandleNextElement()
    {
        return nullptr;
    }

    //LevelXMLParser::LevelXMLParser(const char* pFilePath)
    //{
    //    m_pXmlDoc = std::make_unique<tinyxml2::XMLDocument>();
    //    int error = m_pXmlDoc->LoadFile(pFilePath);
    //    if (error != tinyxml2::XML_SUCCESS)
    //        BB_LOG(LogType::kFailure, "Couldnt load LevelXML file : ", pFilePath, " - ", m_pXmlDoc->ErrorName());
    //    auto* pRoot = m_pXmlDoc->FirstChildElement("Actor");
    //    m_pNextElement = pRoot->FirstChildElement(kActorElementName); // Get next actor

    //    if (!m_pNextElement)
    //        BB_LOG(LogType::kError, "Level file didnt have any actors \"", pFilePath, '\"');
    //}

    LevelXMLParser::LevelXMLParser(tinyxml2::XMLDocument* pDocument)
    {
        if (pDocument->Error())
        {
            BB_LOG(LogType::kError, "Error with passed in document -- ", pDocument->ErrorName());
            return;
        }

        m_pNextElement = pDocument->FirstChildElement("Actor");

        if (!m_pNextElement)
            BB_LOG(LogType::kError, "Passed in level doc didnt have any actors");
    }

    ActorXMLParser LevelXMLParser::GetNextActor()
    {
        assert(HasActors());

        ActorXMLParser retVal(m_pNextElement);

        bool actorInSeperateFile;
        if (m_pNextElement->QueryBoolAttribute(kSeperateFileAttribute, &actorInSeperateFile) == tinyxml2::XML_SUCCESS)
        {
            XMLElementParser parser(m_pNextElement);
            const char* pFilePath;
            parser.GetChildVariable(kFilePathElement, &pFilePath);
            retVal = ResourceManager::GetActorXMLData(pFilePath);
        }

        m_pNextElement = m_pNextElement->NextSiblingElement(kActorElementName);

        return retVal;
    }
    
    //------------------------------------------------------------------------------------------------------------
    // Actor Xml Parser
    //------------------------------------------------------------------------------------------------------------
    //ActorXMLParser::ActorXMLParser(const char* pFileName)
    //{
    //    m_pXmlDoc = std::make_unique<tinyxml2::XMLDocument>();
    //    int error = m_pXmlDoc->LoadFile(pFileName);
    //    if (error != tinyxml2::XML_SUCCESS)
    //        BB_LOG(LogType::kFailure, "Couldnt load ActorXML file : ", pFileName, " - " , m_pXmlDoc->ErrorName() );
    //    auto* pRoot = m_pXmlDoc->FirstChildElement("Actor");
    //    m_pComponentElement = pRoot->FirstChildElement(kComponentElementName);
    //}

    ActorXMLParser::ActorXMLParser(tinyxml2::XMLElement* pRootElement)
        : m_pComponentElement(pRootElement->FirstChildElement(kComponentElementName))
    {
    }

    ActorXMLParser::ActorXMLParser(tinyxml2::XMLDocument* pDocument)
    {
        auto* pRoot = pDocument->FirstChildElement("Actor");
        m_pComponentElement = pRoot->FirstChildElement(kComponentElementName);
    }

    const XMLElementParser ActorXMLParser::NextComponent()
    {
        if (!m_pComponentElement)
            return XMLElementParser(nullptr);
        const XMLElementParser returnElement = XMLElementParser(m_pComponentElement);
        m_pComponentElement = m_pComponentElement->NextSiblingElement(kComponentElementName);
        return returnElement;
    }

    bool ActorXMLParser::SaveActor(const std::unique_ptr<Actor>& pActor,
                                   const char* pActorName,
                                   std::filesystem::path filePath)
    {
        tinyxml2::XMLDocument saveDoc;
        auto* pRootElement = saveDoc.NewElement(pActorName);
        saveDoc.InsertFirstChild(pRootElement);

        for (auto& [id, pComponet] : pActor->m_componentMap)
        {
            XMLElementParser componetParser(pRootElement->InsertNewChildElement(kComponentElementName));
            componetParser.GetTinyXML()->SetAttribute(kComponentAttribute, pComponet->Name());
            pComponet->Save(componetParser);
        }
        
        if (filePath.empty())
        {
            filePath = std::filesystem::path(kActorFilePath) / pActorName;
            filePath.replace_extension(".xml");
        }

        if constexpr ( kPrintSavedActors )
            saveDoc.Print();
        
        int errCode = saveDoc.SaveFile( filePath.string().c_str() );
        if ( errCode == tinyxml2::XML_SUCCESS )
        {
            BB_LOG(LogType::kMessage, "Actor saved name:", pActorName, " - saved to :", filePath );
            return true;
        }
        BB_LOG(LogType::kError, "Failed to save actor file , errorcode : " , errCode , "  " , saveDoc.ErrorStr() );
        return false;
    }

    //------------------------------------------------------------------------------------------------------------
    // XML Element Parser
    //------------------------------------------------------------------------------------------------------------

    XMLElementParser::XMLElementParser(tinyxml2::XMLElement* pElement)
        : m_pRootElement(pElement)
    {

    }

    const char* XMLElementParser::GetComponentName() const
    {
        return m_pRootElement->FirstAttribute()->Value();
    }

    XMLElementParser XMLElementParser::GetChildElement(const char* pName) const
    {
        return XMLElementParser(m_pRootElement->FirstChildElement(pName));
    }

    void XMLElementParser::GetChildVariable(const char* pName, const char** savedVariable) const
    {
        XMLElementParser child = GetChildElement(pName);
        if (!child)
        {
            BB_LOG(LogType::kError, '\"', pName, '\"', " not found");
            return;
        }
        *savedVariable = child->GetText();
    }

    void XMLElementParser::GetChildVariable(const char* pName, int* savedVariable) const
    {
        XMLElementParser child = GetChildElement(pName);
        if(!child)
        {
            BB_LOG(LogType::kError, '\"', pName, '\"', " not found");
            return;
        }
        if (child->QueryIntText(savedVariable) != tinyxml2::XML_SUCCESS)
        {
            BB_LOG(LogType::kError, "Int variable not found , \"", pName, '\"');
            *savedVariable = 0;
        }
    }

    void XMLElementParser::GetChildVariable(const char* pName, unsigned* savedVariable) const
    {
        XMLElementParser child = GetChildElement(pName);
        if (!child)
        {
            BB_LOG(LogType::kError, '\"', pName, '\"', " not found");
            return;
        }
        if (child->QueryUnsignedText(savedVariable) != tinyxml2::XML_SUCCESS)
        {
            BB_LOG(LogType::kError, "u-Int variable not found , \"", pName, '\"');
            *savedVariable = 0;
        }
    }

    void XMLElementParser::GetChildVariable(const char* pName, int8_t* savedVariable) const
    {
        int temp = 0;
        GetChildVariable(pName, &temp);
        *savedVariable = static_cast<int8_t>(temp);
    }

    void XMLElementParser::GetChildVariable(const char* pName, uint8_t* savedVariable) const
    {
        int temp = 0;
        GetChildVariable(pName, &temp);
        *savedVariable = static_cast<uint8_t>(temp);
    }

    void XMLElementParser::GetChildVariable(const char* pName, int16_t* savedVariable) const
    {
        int temp = 0;
        GetChildVariable(pName, &temp);
        *savedVariable = static_cast<int16_t>(temp);
    }

    void XMLElementParser::GetChildVariable(const char* pName, uint16_t* savedVariable) const
    {
        int temp = 0;
        GetChildVariable(pName, &temp);
        *savedVariable = static_cast<uint16_t>(temp);
    }

    void XMLElementParser::GetChildVariable(const char* pName, int64_t* savedVariable) const
    {
        XMLElementParser child = GetChildElement(pName);
        if (!child)
        {
            BB_LOG(LogType::kError, '\"', pName, '\"', " not found");
            return;
        }
        if (child->QueryInt64Text(savedVariable) != tinyxml2::XML_SUCCESS)
        {
            BB_LOG(LogType::kError, "int64 variable not found , \"", pName, '\"');
            *savedVariable = 0;
        }
    }

    void XMLElementParser::GetChildVariable(const char* pName, uint64_t* savedVariable) const
    {
        XMLElementParser child = GetChildElement(pName);
        if (!child)
        {
            BB_LOG(LogType::kError, '\"', pName, '\"', " not found");
            return;
        }
        if (child->QueryUnsigned64Text(savedVariable) != tinyxml2::XML_SUCCESS)
        {
            BB_LOG(LogType::kError, "u-Int64 variable not found , \"", pName, '\"');
            *savedVariable = 0;
        }
    }

    void XMLElementParser::GetChildVariable(const char* pName, bool* savedVariable) const
    {
        XMLElementParser child = GetChildElement(pName);
        if (!child)
        {
            BB_LOG(LogType::kError, '\"', pName, '\"', " not found");
            return;
        }
        if (child->QueryBoolText(savedVariable) != tinyxml2::XML_SUCCESS)
        {
            BB_LOG(LogType::kError, "bool variable not found , \"", pName, '\"');
            *savedVariable = 0;
        }
    }

    void XMLElementParser::GetChildVariable(const char* pName, double* savedVariable) const
    {
        XMLElementParser child = GetChildElement(pName);
        if (!child)
        {
            BB_LOG(LogType::kError, '\"', pName, '\"', " not found");
            return;
        }
        if (child->QueryDoubleText(savedVariable) != tinyxml2::XML_SUCCESS)
        {
            BB_LOG(LogType::kError, "Double variable not found , \"", pName, '\"');
            *savedVariable = 0;
        }
    }

    void XMLElementParser::GetChildVariable(const char* pName, float* savedVariable) const
    {
        XMLElementParser child = GetChildElement(pName);
        if (!child)
        {
            BB_LOG(LogType::kError, '\"', pName, '\"', " not found");
            return;
        }
        if (child->QueryFloatText(savedVariable) != tinyxml2::XML_SUCCESS)
        {
            BB_LOG(LogType::kError, "Float variable not found , \"", pName, '\"');
            *savedVariable = 0;
        }
    }

    void XMLElementParser::ClearChildElements()
    {
        if (m_pRootElement->ChildElementCount() > 0) // ensure the parser is empty.
            m_pRootElement->DeleteChildren();
    }

    XMLElementParser XMLElementParser::InsertNewChild(const char* pName)
    {
        return XMLElementParser(m_pRootElement->InsertNewChildElement(pName));
    }

    void XMLElementParser::NewChildVariable(const char* pName, const char* savedVariable)
    {
        m_pRootElement->InsertNewChildElement(pName)->SetText(savedVariable);
    }

    void XMLElementParser::NewChildVariable(const char* pName, int savedVariable)
    {
        m_pRootElement->InsertNewChildElement(pName)->SetText(savedVariable);
    }

    void XMLElementParser::NewChildVariable(const char* pName, unsigned savedVariable)
    {
        m_pRootElement->InsertNewChildElement(pName)->SetText(savedVariable);
    }

    void XMLElementParser::NewChildVariable(const char* pName, uint8_t savedVariable)
    {
        NewChildVariable(pName, static_cast<int>(savedVariable));
    }

    void XMLElementParser::NewChildVariable(const char* pName, int8_t savedVariable)
    {
        NewChildVariable(pName, static_cast<int>(savedVariable));
    }

    void XMLElementParser::NewChildVariable(const char* pName, uint16_t savedVariable)
    {
        NewChildVariable(pName, static_cast<int>(savedVariable));
    }

    void XMLElementParser::NewChildVariable(const char* pName, int16_t savedVariable)
    {
        NewChildVariable(pName, static_cast<int>(savedVariable));
    }

    void XMLElementParser::NewChildVariable(const char* pName, int64_t savedVariable)
    {
        m_pRootElement->InsertNewChildElement(pName)->SetText(savedVariable);
    }

    void XMLElementParser::NewChildVariable(const char* pName, uint64_t savedVariable)
    {
        m_pRootElement->InsertNewChildElement(pName)->SetText(savedVariable);
    }

    void XMLElementParser::NewChildVariable(const char* pName, bool savedVariable)
    {
        m_pRootElement->InsertNewChildElement(pName)->SetText(savedVariable);
    }

    void XMLElementParser::NewChildVariable(const char* pName, double savedVariable)
    {
        m_pRootElement->InsertNewChildElement(pName)->SetText(savedVariable);
    }

    void XMLElementParser::NewChildVariable(const char* pName, float savedVariable)
    {
        m_pRootElement->InsertNewChildElement(pName)->SetText(savedVariable);
    }

}