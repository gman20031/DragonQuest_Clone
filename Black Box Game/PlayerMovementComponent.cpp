#include "PlayerMovementComponent.h"
#include "../Black Box Engine/BlackBoxManager.h"
#include "../Black Box Engine/Input/InputManager.h"
#include "../Black Box Engine/Actors/Collision/CollisionManager.h"
#include "../Black Box Engine/Actors/EngineComponents/MovementBlocker.h"
#include "TileSystem/TileInfoComponent.h"

using namespace BlackBoxEngine;

#include <System/Delay.h>

PlayerMovementComponent::PlayerMovementComponent(BlackBoxEngine::Actor* pOwner)
	: Component(pOwner)
    , m_targetPosition(0.0f, 0.0f)
{

}

PlayerMovementComponent::~PlayerMovementComponent()
{
    using enum InputManager::InputType;
    // bad and annoying - remembering codes suck
    auto* pInputManager = BlackBoxManager::Get()->m_pInputManager;
    for (int i = 0; i < 4; ++i)
        pInputManager->UnsubscribeKey(m_callbackCodes[i], kKeyDown);
    for (int i = 4; i < 8; ++i)
        pInputManager->UnsubscribeKey(m_callbackCodes[i], kKeyUp);
}

void PlayerMovementComponent::Start()
{
    m_pTransform = m_pOwner->GetComponent<TransformComponent>();
    m_pMover = m_pOwner->GetComponent<MoverComponent>();
    InputManager* pInputManager = BlackBoxManager::Get()->m_pInputManager;
    using enum InputManager::InputType;
    int index = 0;

    const auto& pTileActor = BlackBoxManager::Get()->m_pActorManager->GetActor(m_tileMapId);
    m_pTileMap = pTileActor->GetComponent<TileMapComponent>();
    // insane monster of shit just to be able to make input delayed.
    const auto registerDownKey = [this, pInputManager, &index](KeyCode keyCode, float x, float y)
    {
        m_callbackCodes[index++] = pInputManager->SubscribeToKey(keyCode, kKeyDown, [this,keyCode, x,y]()
        {
            SetTextureForDirection({x,y});
            Delay(200, [this, keyCode, x, y]()
                {
                    auto* pInput = BlackBoxManager::Get()->m_pInputManager;
                    if (pInput->IsKeyDown(keyCode))
                        TryMove({ x, y });
                });
        } );
    };
    registerDownKey(kUpKey, 0, -1);
    registerDownKey(kDownKey, 0, +1);
    registerDownKey(kLeftKey, -1, 0);
    registerDownKey(kRightKey, +1, 0);

    // Stop moving
    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kUpKey, kKeyUp, [this]() {
        StopMoving();
    });
    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kDownKey, kKeyUp, [this]() {
        StopMoving();
    });
    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kLeftKey, kKeyUp, [this]() {
        StopMoving();
    });
    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kRightKey, kKeyUp, [this]() {
        StopMoving();
    });
}

void PlayerMovementComponent::Update()
{
    if (!m_isMoving)
        return;

    float deltaTime = static_cast<float>(BlackBoxManager::Get()->GetDeltaTime());
    FVector2& position = m_pTransform->m_position;
    const FVector2 toTarget = m_targetPosition - position;

    // Check if we've reached or passed the target tile (allow some floating point tolerance)
    if (toTarget.GetLength() <= m_playerSpeed * deltaTime)
    {
        // Snap position to target tile exactly
        position = m_targetPosition;

        // Stop movement
        if(m_stopMoving)
        {
            m_pMover->m_velocity = FVector2(0, 0);
            m_isMoving = false;
        }
        else
            SetTargetTile();
    }
}

void PlayerMovementComponent::Render()
{
}

void PlayerMovementComponent::OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{
}

void PlayerMovementComponent::Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser)
{
    parser.NewChildVariable("Speed", m_playerSpeed);
    parser.NewChildVariable("TileMapID", 0);
}

void PlayerMovementComponent::Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser)
{
    parser.GetChildVariable("Speed", &m_playerSpeed);
    parser.GetChildVariable("TileMapID", &m_tileMapId);
}

void PlayerMovementComponent::TryMove(const FVector2& direction)
{
    if (m_isMoving)
        return; // Already moving; ignore input

    m_stopMoving = false;
    m_direction = direction;

    // I will make this easier to work with, but for now it is literally sdl3
    DelayFunction callback = [](void* pData, [[maybe_unused]]uint32_t timerId, [[maybe_unused]] uint32_t interval)->uint32_t
        {
            auto* pMovementComponent = static_cast<PlayerMovementComponent*>(pData);
            pMovementComponent->SetTargetTile();
            return 0;
        };
    Delay(250, callback, this);
}

void PlayerMovementComponent::SetTextureForDirection([[maybe_unused]]const BlackBoxEngine::FVector2& direction)
{
    // does nothing right now
}

void PlayerMovementComponent::SetTargetTile()
{
    // Calculate target position by moving one tile in the direction
    FVector2 currentPos = m_pTransform->m_position;
    FVector2 desiredTarget = currentPos + m_direction * TILE_SIZE;

    uint32_t tileX = static_cast<uint32_t>(desiredTarget.x / TILE_SIZE);
    uint32_t tileY = static_cast<uint32_t>(desiredTarget.y / TILE_SIZE);
    
    
    if (!m_pTileMap)
    {
        StopMoving();
        return;
    }
    
    if (tileX >= m_pTileMap->GetMapWidth() || tileY >= m_pTileMap->GetMapHeight())
    {
        StopMoving();
        return;
    }

    const auto& tileActor = m_pTileMap->GetTileAt(tileX, tileY);
    if (!tileActor)
    {
        StopMoving();
        return;
    }
    
    auto tileInfo = tileActor->GetComponent<TileInfoComponent>();
    if (!tileInfo || !tileInfo->IsWalkable())
    {
        StopMoving();
        return;
    }
    
    m_targetPosition = desiredTarget;
    m_isMoving = true;

    // Set velocity toward the target position
    FVector2 toTarget = m_targetPosition - currentPos;
    FVector2 normalizedDirection = toTarget.GetNormalizedVector();
    m_pMover->m_velocity = normalizedDirection * m_playerSpeed;
}

void PlayerMovementComponent::StopMoving()
{
    m_stopMoving = true;
}
