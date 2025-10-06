#pragma once

#include <memory>
#include <stdint.h>
#include <filesystem>

#include "../../Graphics/RenderingStructs.h"

namespace tinyxml2
{
    class XMLDocument;
    class XMLElement;
}

namespace BlackBoxEngine
{
    class Actor;

    /**
     * @brief simple wrapper for a tinyxml2::XMLElement*
     * @brief intended for both reading and writing component info
     */
    class XMLElementParser
    {
        tinyxml2::XMLElement* m_pRootElement;
        tinyxml2::XMLElement* operator->() { return m_pRootElement; }
    public:
        XMLElementParser(tinyxml2::XMLElement* pElement);
        tinyxml2::XMLElement* GetTinyXML() { return m_pRootElement; }
        operator bool() const { return m_pRootElement; }

        void ClearChildElements();
        XMLElementParser FindSiblingElement(const char* pName = nullptr);
        XMLElementParser GetSibling();
        void GetElementName(const char** ppName);
        void GetAttribute(const char* pName, const char** ppText);
        void GetText( const char** ppText );
        void GetInt(int* pInt);

        /// Loading interface

        const char* GetFirstAttribute() const;
        XMLElementParser GetChildElement(const char* pName = nullptr) const;
        void GetChildVariable(const char* pName, std::string* savedVariable) const;
        void GetChildVariable(const char* pName, const char** savedVariable) const;
        void GetChildVariable(const char* pName, int* savedVariable) const;
        void GetChildVariable(const char* pName, unsigned* savedVariable) const;
        void GetChildVariable(const char* pName, int8_t* savedVariable) const;
        void GetChildVariable(const char* pName, uint8_t* savedVariable) const;
        void GetChildVariable(const char* pName, int16_t* savedVariable) const;
        void GetChildVariable(const char* pName, uint16_t* savedVariable) const;
        void GetChildVariable(const char* pName, int64_t* savedVariable) const;
        void GetChildVariable(const char* pName, uint64_t* savedVariable) const;
        void GetChildVariable(const char* pName, bool* savedVariable) const;
        void GetChildVariable(const char* pName, double* savedVariable) const;
        void GetChildVariable(const char* pName, float* savedVariable) const;
        void GetChildVariable(const char* pName, BB_AnchorPoint* savedVariable) const;

        /// Saving interface 

        XMLElementParser InsertNewChild(const char* pName);
        void NewChildVariable(const char* pName, const std::string& savedVariable);
        void NewChildVariable(const char* pName, const char* savedVariable);
        void NewChildVariable(const char* pName, int savedVariable);
        void NewChildVariable(const char* pName, unsigned savedVariable);
        void NewChildVariable(const char* pName, uint8_t savedVariable);
        void NewChildVariable(const char* pName, int8_t savedVariable);
        void NewChildVariable(const char* pName, uint16_t savedVariable);
        void NewChildVariable(const char* pName, int16_t savedVariable);
        void NewChildVariable(const char* pName, int64_t savedVariable);
        void NewChildVariable(const char* pName, uint64_t savedVariable);
        void NewChildVariable(const char* pName, bool savedVariable);
        void NewChildVariable(const char* pName, double savedVariable);
        void NewChildVariable(const char* pName, float savedVariable);
        void NewChildVariable(const char* pName, BB_AnchorPoint savedVariable);

    };

    class ActorXMLParser
    {

        tinyxml2::XMLElement* m_pComponentElement = nullptr;
    public:
        inline static constexpr bool kPrintSavedActors = false;
        inline static constexpr const char* kActorElementName = "Actor";
        inline static constexpr const char* kNameAttribute = "Name";
        inline static constexpr const char* kComponentElementName = "Component";
        inline static constexpr const char* kActorFilePath = "../Assets/Actors/";

        ActorXMLParser() = default;
        ActorXMLParser(tinyxml2::XMLElement* pRootElement);
        ActorXMLParser(tinyxml2::XMLDocument* pDocument);

        const XMLElementParser NextComponent();

        static bool SaveActor( const std::unique_ptr<Actor>& pActor, const char* pActorName, std::filesystem::path filePath = {} );
        static bool SaveActor(const Actor* pActor, const char* pActorName, std::filesystem::path filePath = {});
    };

    class LevelXMLParser
    {
        inline static constexpr const char* kSeperateFileAttribute = "SeperateFile";
        inline static constexpr const char* kFilePathElement = "FilePath";
        inline static constexpr const char* kLevelFilePath = "../Assets/Actors/";

        tinyxml2::XMLElement* m_pNextElement = nullptr;
    
    private:
        tinyxml2::XMLElement* HandleNextElement();
    public:
        LevelXMLParser(tinyxml2::XMLDocument*);

        ActorXMLParser GetNextActor();
        bool HasActors() const { return m_pNextElement; };
    };








}
