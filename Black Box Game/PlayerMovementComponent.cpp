#include "PlayerMovementComponent.h"
#include "../Black Box Engine/BlackBoxManager.h"

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
    for(auto id : m_keyDownCodes)
        pInputManager->UnsubscribeKey(id, kKeyDown);
    for (auto id : m_keyUpCodes)
        pInputManager->UnsubscribeKey(id, kKeyUp);
}

void PlayerMovementComponent::Start()
{
    m_pTransform = m_pOwner->GetComponent<TransformComponent>();
    m_pMover = m_pOwner->GetComponent<MoverComponent>();

    m_pAnimatedSprite = m_pOwner->GetComponent<AnimatedSpriteComponent>();
    if (!m_pAnimatedSprite)
        BB_LOG(LogType::kError, "PlayerMovementComponent: No AnimatedSpriteComponent attached!");

    InputManager* pInputManager = BlackBoxManager::Get()->m_pInputManager;
    using enum InputManager::InputType;
    int index = 0;

    const auto& pTileActor = BlackBoxManager::Get()->m_pActorManager->GetActor(m_tileMapId);
    m_pTileMap = pTileActor->GetComponent<TileMapComponent>();

     //insane monster of shit just to be able to make input delayed.
    const auto registerDownKey = [this, pInputManager, &index](KeyCode keyCode, float x, float y)
    {
        uint64_t id = pInputManager->SubscribeToKey(keyCode, kKeyDown, [this,keyCode, x,y]()
        {
            SetTextureForDirection({x,y});

            Delay(200, [this, keyCode, x, y]()
                {
                    auto* pInput = BlackBoxManager::Get()->m_pInputManager;
                    if (pInput->IsKeyDown(keyCode))
                        TryMove({ x, y });
            
                    return 0;
                });
        } );
        m_keyDownCodes.emplace_back(id);
    };

    registerDownKey(kUpKey, 0, -1);
    registerDownKey(kDownKey, 0, +1);
    registerDownKey(kLeftKey, -1, 0);
    registerDownKey(kRightKey, +1, 0);

    // Stop moving
    m_keyUpCodes.emplace_back(pInputManager->SubscribeToKey(kUpKey, kKeyUp, [this]() {
        StopMoving();
    }));
    m_keyUpCodes.emplace_back(pInputManager->SubscribeToKey(kDownKey, kKeyUp, [this]() {
        StopMoving();
    }));
    m_keyUpCodes.emplace_back(pInputManager->SubscribeToKey(kLeftKey, kKeyUp, [this]() {
        StopMoving();
    }));
    m_keyUpCodes.emplace_back(pInputManager->SubscribeToKey(kRightKey, kKeyUp, [this]() {
        StopMoving();
    }));
}


void PlayerMovementComponent::Update()
{
    float deltaTime = static_cast<float>(BlackBoxManager::Get()->GetDeltaTime());
    FVector2& position = m_pTransform->m_position;
    const FVector2 toTarget = m_targetPosition - position;

    if (!m_isMoving)
        return;

    // Check if we've reached or passed the target tile (allow some floating point tolerance)
    if (toTarget.GetLength() <= m_playerSpeed * deltaTime)
    {
        // Snap position to target tile exactly
        position = m_targetPosition;
        CheckForEncounters();

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

void PlayerMovementComponent::SetAnimationPaused(bool paused)
{
    if (paused)
        m_pAnimatedSprite->Sprite().StopAnimating();

    else
        m_pAnimatedSprite->Sprite().AnimateSprite(2, 1);
}

void PlayerMovementComponent::TryMove(const FVector2& direction)
{
    if (m_isMoving)
        return; // Already moving; ignore input

    m_stopMoving = false;
    m_direction = direction;
    SetTargetTile();

}

void PlayerMovementComponent::SetTextureForDirection([[maybe_unused]]const BlackBoxEngine::FVector2& direction)
{
    if (!m_pAnimatedSprite)
        return;
    
    int startIndex = 0;
    int endIndex = 1;
    
    if (direction.y < 0)        // Up
    {
        startIndex = 4;
        endIndex = 5;
    }
    else if (direction.y > 0)   // Down
    {
        startIndex = 0;
        endIndex = 1;
    }
    else if (direction.x > 0)   // Right
    {
        startIndex = 6;
        endIndex = 7;
    }
    else if (direction.x < 0)   // Left
    {
        startIndex = 2;
        endIndex = 3;
    }

    if (m_pAnimatedSprite->Sprite().GetAnimationStartIndex() == startIndex && m_pAnimatedSprite->Sprite().GetAnimationEndIndex() == endIndex)
        return;
    
    else
    {
        m_pAnimatedSprite->Sprite().StopAnimating();

        m_pAnimatedSprite->Sprite().SetAnimationStartIndex(startIndex);
        m_pAnimatedSprite->Sprite().SetAnimationEndIndex(endIndex);
    }
    
    m_pAnimatedSprite->Sprite().AnimateSprite(2, 1);
    
}

void PlayerMovementComponent::CheckForEncounters()
{
    auto* pInfo = m_pTileMap->GetTileAtGamePosition( m_targetPosition )->GetComponent<TileInfoComponent>();
    bool encounter = pInfo->CheckEncounter();
    if ( !encounter )
        return;
    m_pTileMap->GetEncounterAtGame( m_targetPosition );
}

void PlayerMovementComponent::SetTargetTile()
{
    if (!m_pTileMap)
    {
        BB_LOG(LogType::kError, "No tilemap attached to playerMovementComponent");
        return;
    }

    // Calculate target position by moving one tile in the direction
    float tileSize = static_cast<float>(m_pTileMap->GetTileSize());
    FVector2 currentPos = m_pTransform->m_position;
    FVector2 desiredTarget = (currentPos) + (m_direction * tileSize);

    // check if we can walk on that tile
    const auto& tileActor = m_pTileMap->GetTileAtGamePosition(desiredTarget);
    auto* pTileInfo = tileActor->GetComponent<TileInfoComponent>();

    if (!pTileInfo || !pTileInfo->IsWalkable())
    {
        m_isMoving = false;
        m_targetPosition = m_pTransform->m_position;
        m_pMover->m_velocity = { 0,0 };
        return;
    }

    // set movement target
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
