#pragma once

#include <Actors/Component.h>

class ExampleComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("ExampleComponent");

    int m_bar = 25;
    const char* m_baz = "Baz";
    std::string m_foo = "foo";

public:
    ExampleComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) { /*EMPTY*/ }
    virtual ~ExampleComponent() = default;

    virtual void Start() override; // only when this is created.
    virtual void Update() override; // every tick of the game
    virtual void Render() override; // just for rendering stuff
    virtual void OnCollide(BlackBoxEngine::Actor* pOtherActor) override; // if this actor has a collider, and walks into another actor with a collider
    virtual void Save(BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be saved
    virtual void Load(const BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be loaded
    
};
