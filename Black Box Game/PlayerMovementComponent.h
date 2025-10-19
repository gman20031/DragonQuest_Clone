#pragma once

#include <Actors/EngineComponents/MoverComponent.h>
#include <Actors/EngineComponents/TransformComponent.h>
#include "../Black Box Engine/Input/InputManager.h"
#include "TileSystem/TileMapComponent.h"
#include "../Black Box Engine/Math/FVector2.h" // why are we using "../" here and not <>
#include "../Black Box Engine/Actors/EngineComponents/SpriteComponent.h"
#include "InteractionComponent.h"

class PlayerMovementComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("PlayerMovementComponent");
    friend class PlayerStatsComponent;
    friend class EncounterComponent;

    BlackBoxEngine::MoverComponent* m_pMover = nullptr;
    BlackBoxEngine::TransformComponent* m_pTransform = nullptr;
    InteractionComponent* m_pInteraction = nullptr;

    int m_tileMapId = 0;
    TileMapComponent* m_pTileMap = nullptr;

    std::vector<uint64_t> m_keyDownCodes;
    std::vector<uint64_t> m_keyUpCodes;
    static constexpr BlackBoxEngine::KeyCode kUpKey = BlackBoxEngine::KeyCode::kUp;
    static constexpr BlackBoxEngine::KeyCode kLeftKey = BlackBoxEngine::KeyCode::kLeft;
    static constexpr BlackBoxEngine::KeyCode kDownKey = BlackBoxEngine::KeyCode::kDown;
    static constexpr BlackBoxEngine::KeyCode kRightKey = BlackBoxEngine::KeyCode::kRight;

    float m_playerSpeed = 180;
    bool m_isMoving = false;
    bool m_stopMoving = false;
    BlackBoxEngine::FVector2 m_direction{ 0, 0 };
    BlackBoxEngine::FVector2 m_targetPosition; // in world coordinates

    bool m_isContinuous = false;
    bool m_waitingForTap = false;
    BlackBoxEngine::KeyCode m_lastKeyCode{};

    BlackBoxEngine::AnimatedSpriteComponent* m_pAnimatedSprite = nullptr;

    int32_t m_uiCount = 0;

private:
    void SetTargetTile();
    void StopMoving();
    void TryMove(const BlackBoxEngine::FVector2& direction);
    void SetTextureForDirection(const BlackBoxEngine::FVector2& direction);
    void CheckForEncounters();
    void AddUIMessageCallbacks();

public:
    PlayerMovementComponent(BlackBoxEngine::Actor* pOwner);
    virtual ~PlayerMovementComponent();

    virtual void Start() override; // only when this is created.
    virtual void Update() override; // every tick of the game
    virtual void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be saved
    virtual void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override; // for when this actor is called to be loaded

    void SetInteractionComponent(InteractionComponent* interaction);
    void SetAnimationPaused(bool paused);
};

