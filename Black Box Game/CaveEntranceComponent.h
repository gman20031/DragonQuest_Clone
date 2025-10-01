#pragma once

#include <Actors/Component.h>


class BlackBoxGame;

class CaveEntranceComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("CaveEntranceComponent");

public:
    void OnInteract(); 

    CaveEntranceComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) { /*EMPTY*/ }
    virtual ~CaveEntranceComponent() = default;

    virtual void Start() override; // only when this is created.
    virtual void Update() override; // every tick of the game
    virtual void Render() override; // just for rendering stuff
    virtual void OnCollide(BlackBoxEngine::Actor* pOtherActor) override; // if this actor has a collider, and walks into another actor with a collider
    virtual void Save(BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be saved
    virtual void Load(const BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be loaded
};

