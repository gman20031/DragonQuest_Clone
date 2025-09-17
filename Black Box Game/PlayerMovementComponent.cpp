#include "PlayerMovementComponent.h"
#include "../Black Box Engine/BlackBoxManager.h"
#include "../Black Box Engine/Input/InputManager.h"
#include "../Black Box Engine/Actors/Collision/CollisionManager.h"
#include "../Black Box Engine/Actors/EngineComponents/MovementBlocker.h"

using namespace BlackBoxEngine;

PlayerMovementComponent::PlayerMovementComponent(BlackBoxEngine::Actor* pOwner)
	: Component(pOwner)
{
	m_pTransform = pOwner->GetComponent<TransformComponent>();
	m_pMover = pOwner->GetComponent<MoverComponent>();
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
    auto* pInputManager = BlackBoxManager::Get()->m_pInputManager;
    using enum InputManager::InputType;
    int index = 0;

    // Key Down
    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kUpKey, kKeyDown,
        [this]() {m_pMover->m_velocity.y -= kPlayerSpeed; }
    );

    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kDownKey, kKeyDown,
        [this]() {m_pMover->m_velocity.y += kPlayerSpeed; }
    );

    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kRightKey, kKeyDown,
        [this]() {m_pMover->m_velocity.x += kPlayerSpeed; }
    );

    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kLeftKey, kKeyDown,
        [this]() {m_pMover->m_velocity.x -= kPlayerSpeed; }
    );

    // Key Up
    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kUpKey, kKeyUp,
        [this]() {m_pMover->m_velocity.y += kPlayerSpeed; }
    );

    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kDownKey, kKeyUp,
        [this]() {m_pMover->m_velocity.y -= kPlayerSpeed; }
    );

    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kRightKey, kKeyUp,
        [this]() {m_pMover->m_velocity.x -= kPlayerSpeed; }
    );

    m_callbackCodes[index++] = pInputManager->SubscribeToKey(kLeftKey, kKeyUp,
        [this]() {m_pMover->m_velocity.x += kPlayerSpeed; }
    );
}

void PlayerMovementComponent::Update()
{
}

void PlayerMovementComponent::Render()
{
}

void PlayerMovementComponent::OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{
}

void PlayerMovementComponent::Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser)
{
}

void PlayerMovementComponent::Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser)
{
}


