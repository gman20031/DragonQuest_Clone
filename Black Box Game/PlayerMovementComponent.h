#pragma once
#include <Actors/Component.h>
#include <Actors/ActorManager.h>
#include <Actors/EngineComponents/MoverComponent.h>
#include <Actors/EngineComponents/TransformComponent.h>
#include <Input/InputManager.h>

class PlayerMovementComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("PlayerMovementComponent");

    BlackBoxEngine::MoverComponent* m_pMover;
    BlackBoxEngine::TransformComponent* m_pTransform;

    uint64_t m_callbackCodes[8] = {};
    static constexpr BlackBoxEngine::KeyCode kUpKey = BlackBoxEngine::KeyCode::kUp;
    static constexpr BlackBoxEngine::KeyCode kLeftKey = BlackBoxEngine::KeyCode::kLeft;
    static constexpr BlackBoxEngine::KeyCode kDownKey = BlackBoxEngine::KeyCode::kDown;
    static constexpr BlackBoxEngine::KeyCode kRightKey = BlackBoxEngine::KeyCode::kRight;

    float m_playerSpeed = 180;

public:
    PlayerMovementComponent(BlackBoxEngine::Actor* pOwner);
    ~PlayerMovementComponent();

    virtual void Start() override; // only when this is created.
    virtual void Update() override; // every tick of the game
    virtual void Render() override; // just for rendering stuff
    virtual void OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor) override; // if this actor has a collider, and walks into another actor with a collider
    virtual void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be saved
    virtual void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be loaded

};

