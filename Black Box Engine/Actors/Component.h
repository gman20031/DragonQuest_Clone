#pragma once
#include <concepts>
#include <string>
#include "../System/StringHash.h"
#include "../Resources/XML/BlackBoxXMLParser.h"

namespace BlackBoxEngine
{
    class Actor;
    ////////////////////////////////////////////////////////////
    // General Component class
    //   constexpr void Start() {}
    //   void Start() {}
    //   void Render() {}
    //   void Update() {}
    //   void OnCollide(Actor* pOther) {}
    //   void Save (const XMLElementParser parser) {}
    //   void Load (XMLElementParser parser) {}
    //	To inherit:
    //   add GENERATE_ID 
    //   and Esnure constructor calls component constructor
    ////////////////////////////////////////////////////////////
    class Component
    {
    public:
        using Id = uint32_t;
    protected:
        Actor* m_pOwner;
    public:
        Component(Actor* pOwner) : m_pOwner(pOwner) { /* EMPTY */ }
        virtual ~Component() = default;
        Component(Component&&) = default;
        Component(const Component&) = delete;
        Component& operator=(const Component&) = delete;
        Component& operator=(Component&&) = default;

        constexpr virtual const char* Name() const { return "Component"; }
        constexpr virtual void Start() { /* EMPTY */ }
        constexpr virtual void Render() { /* EMPTY */ }
        constexpr virtual void Update() { /* EMPTY */ }
        constexpr virtual void OnCollide([[maybe_unused]] Actor* pOther) { /* EMPTY */ }
        virtual void Save([[maybe_unused]]XMLElementParser parser) { /* EMPTY */ }
        virtual void Load([[maybe_unused]]const XMLElementParser parser) { /* EMPTY */ }

        Actor* GetOwner() const { return m_pOwner; }
    };

    ////////////////////////////////////////////////////////////
    // Concept equivilant to - std::derived_from<T, Component>
    ////////////////////////////////////////////////////////////
    template<class T>
    concept SubComponent = std::derived_from<T, Component>;

#define GENERATE_ID(name_c_string)	\
public: static inline constexpr Component::Id s_id = BlackBoxEngine::StringHash(name_c_string);\
        static inline constexpr const char* s_name = name_c_string;\
        virtual const char* Name() const override {return name_c_string;} private:

};
