#include "PlayerMovementComponent.h"
#include "../Black Box Engine/BlackBoxManager.h"
#include "../Black Box Engine/Input/InputManager.h"
#include "../Black Box Engine/Actors/Collision/CollisionManager.h"
#include "../Black Box Engine/Actors/EngineComponents/MovementBlocker.h"
//#include "../Black Box Engine/Math/FVector2.h"

using namespace BlackBoxEngine;


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
   //for (int i = 0; i < 4; ++i)
   //    pInputManager->UnsubscribeKey(m_callbackCodes[i], kKeyDown);
   //for (int i = 4; i < 8; ++i)
   //    pInputManager->UnsubscribeKey(m_callbackCodes[i], kKeyUp);

   for (int i = 0; i < 4; ++i)
       pInputManager->UnsubscribeKey(m_callbackCodes[i], kKeyDown);
}

void PlayerMovementComponent::Start()
{
    m_pTransform = m_pOwner->GetComponent<TransformComponent>();
    m_pMover = m_pOwner->GetComponent<MoverComponent>();
    auto* pInputManager = BlackBoxManager::Get()->m_pInputManager;
    using enum InputManager::InputType;
    int index = 0;

    // Key Down
   //m_callbackCodes[index++] = pInputManager->SubscribeToKey(kUpKey, kKeyDown,
   //    [this]() {m_pMover->m_velocity.y -= m_playerSpeed; }
   //);
   //
   //m_callbackCodes[index++] = pInputManager->SubscribeToKey(kDownKey, kKeyDown,
   //    [this]() {m_pMover->m_velocity.y += m_playerSpeed; }
   //);
   //
   //m_callbackCodes[index++] = pInputManager->SubscribeToKey(kRightKey, kKeyDown,
   //    [this]() {m_pMover->m_velocity.x += m_playerSpeed; }
   //);
   //
   //m_callbackCodes[index++] = pInputManager->SubscribeToKey(kLeftKey, kKeyDown,
   //    [this]() {m_pMover->m_velocity.x -= m_playerSpeed; }
   //);
   //
   ////// Key Up
   //m_callbackCodes[index++] = pInputManager->SubscribeToKey(kUpKey, kKeyUp,
   //    [this]() {m_pMover->m_velocity.y += m_playerSpeed; }
   //);
   //
   //m_callbackCodes[index++] = pInputManager->SubscribeToKey(kDownKey, kKeyUp,
   //    [this]() {m_pMover->m_velocity.y -= m_playerSpeed; }
   //);
   //
   //m_callbackCodes[index++] = pInputManager->SubscribeToKey(kRightKey, kKeyUp,
   //    [this]() {m_pMover->m_velocity.x -= m_playerSpeed; }
   //);
   //
   //m_callbackCodes[index++] = pInputManager->SubscribeToKey(kLeftKey, kKeyUp,
   //    [this]() {m_pMover->m_velocity.x += m_playerSpeed; }
   //);
      // Only subscribe to key down for movement commands
    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kUpKey, kKeyDown, [this]() {
        TryMove({ 0, -1 });
        });
    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kDownKey, kKeyDown, [this]() {
        TryMove({ 0, 1 });
        });
    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kLeftKey, kKeyDown, [this]() {
        TryMove({ -1, 0 });
        });
    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kRightKey, kKeyDown, [this]() {
        TryMove({ 1, 0 });
        });
}

void PlayerMovementComponent::Update()
{
    if (!m_isMoving)
        return;

    // Move position based on velocity and delta time
    float deltaTime = static_cast<float>(BlackBoxManager::Get()->GetDeltaTime());
    FVector2& position = m_pTransform->m_position;
    position += m_pMover->m_velocity * deltaTime;

    FVector2 toTarget = m_targetPosition - position;

    // Check if we've reached or passed the target tile (allow some floating point tolerance)
    if (toTarget.GetLength() <= m_playerSpeed * deltaTime)
    {
        // Snap position to target tile exactly
        position = m_targetPosition;

        // Stop movement
        m_pMover->m_velocity = FVector2(0, 0);
        m_isMoving = false;
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
}

void PlayerMovementComponent::Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser)
{
    parser.GetChildVariable("Speed", &m_playerSpeed);
}

void PlayerMovementComponent::TryMove(const FVector2& direction)
{
    if (m_isMoving)
        return; // Already moving; ignore input

    // Calculate target position by moving one tile in the direction
    FVector2 currentPos = m_pTransform->m_position;
    FVector2 desiredTarget = currentPos + direction * TILE_SIZE;

    // TODO: Add collision check here to see if the target tile is walkable

    m_targetPosition = desiredTarget;
    m_isMoving = true;

    // Set velocity toward the target position
    FVector2 toTarget = m_targetPosition - currentPos;
    FVector2 normalizedDirection = toTarget / toTarget.GetLength();
    m_pMover->m_velocity = normalizedDirection * m_playerSpeed;
}