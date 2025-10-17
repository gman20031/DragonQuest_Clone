#include "StairComponent.h"

#include <chrono>
#include <BlackBoxManager.h>
#include <Graphics/ScreenFader.h>
#include <Actors/EngineComponents/TransformComponent.h>
#include <Actors/EngineComponents/MoverComponent.h>
#include <System/Delay.h>

#include "BlackBoxGame.h"
#include "InteractionComponent.h"
#include "PlayerStatsComponent.h"

using namespace BlackBoxEngine;

void BaseStairComponent::OnCollide(Actor* pOtherActor)
{
    if (!pOtherActor)
        return;

    if (auto* pInteract = pOtherActor->GetComponent<InteractionComponent>())
    {
        pInteract->SetCurrentStair(this);
        pInteract->SetPlayerActor(pOtherActor);
        BB_LOG(LogType::kMessage, "Player collided with a stair leading to %s",
            m_data.targetLevelPath.c_str());

        if (m_data.autoUse)
        {
            BB_LOG(LogType::kMessage, "Auto-using stair to %s", m_data.targetLevelPath.c_str());
            OnStairUsed(pOtherActor);
        }
    }
}

void BaseStairComponent::Save(XMLElementParser parser)
{
    parser.NewChildVariable("TargetLevel", m_data.targetLevelPath);
    parser.NewChildVariable("SpawnX", m_data.newPosition.x);
    parser.NewChildVariable("SpawnY", m_data.newPosition.y);
    parser.NewChildVariable("Fade", m_data.fadeDuration);
    parser.NewChildVariable("AutoUse", m_data.autoUse);
}

void BaseStairComponent::Load(const XMLElementParser parser)
{
    TransitionData data;

    parser.GetChildVariable("TargetLevel", &data.targetLevelPath);
    parser.GetChildVariable("SpawnX", &data.newPosition.x);
    parser.GetChildVariable("SpawnY", &data.newPosition.y);
    parser.GetChildVariable("Fade", &data.fadeDuration);
    parser.GetChildVariable("AutoUse", &data.autoUse);

    SetTransitionData(data);
}

void BaseStairComponent::OnStairUsed(Actor* pOtherActor)
{
    if (!pOtherActor)
        return;

    BB_LOG(LogType::kMessage, "Transitioning to: %s", m_data.targetLevelPath.c_str());

    auto* pInteract = pOtherActor->GetComponent<InteractionComponent>();
    if (!pInteract) return;

    auto* pTransform = pOtherActor->GetComponent<TransformComponent>();
    if (!pTransform) return;

    auto* pManager = BlackBoxManager::Get();
    if (!pManager) return;

    auto* pHUD = pOtherActor->GetComponent<PlayerStatsComponent>();
    if (!pHUD) return;

    // Begin transition
    pInteract->OnLevelTransitionStart();
    pManager->m_pInputManager->StopAllInput();
    pHUD->HideHUD();

    ScreenFader::FadeToBlack(m_data.fadeDuration);

    // Temporarily move and save player
    const auto currentPos = pTransform->m_position;
    pTransform->m_position = m_data.newPosition;
    pTransform->m_prevPosition = m_data.newPosition;

    pManager->m_pActorManager->SaveActor(pOtherActor, "PlayerActor", "../Assets/Actors/PlayerActor.xml");

    // Restore position before fade completes
    pTransform->m_position = currentPos;

    auto delayFunc = [pManager, pOtherActor, pInteract, data = m_data]() -> void
        {
            pManager->m_pInputManager->SwapInputToGame();
            pManager->m_pActorManager->LoadLevel(data.targetLevelPath.c_str());
            pManager->m_pInputManager->ResumeInput();

            ScreenFader::FadeIn(data.fadeDuration);
            pInteract->OnLevelTransitionEnd();
        };

    DelayedCallbackManager::AddCallback(delayFunc, std::chrono::milliseconds(int(m_data.fadeDuration * 1000)));
}
