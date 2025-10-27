#include "StairComponent.h"

#include <chrono>
#include <BlackBoxManager.h>
#include <Graphics/ScreenFader.h>
#include <Actors/EngineComponents/TransformComponent.h>
#include <System/Delay.h>

#include "../BlackBoxGame.h"
#include "../PlayerStatsComponent.h"
#include "InteractionComponent.h"
#include "../GameMessages.h"

using namespace BlackBoxEngine;

BaseStairComponent::~BaseStairComponent()
{
    BlackBoxManager::Get()->m_pMessagingManager->RemoveListener( m_messageId );
}

void BaseStairComponent::Start()
{
    m_messageId = BlackBoxManager::Get()->m_pMessagingManager->RegisterListener( kPlayerMoveStarted,
        [this](Message& message)
        {
            if ( auto* pInteract = message.pSender->GetComponent<InteractionComponent>() )
                pInteract->SetCurrentStair( nullptr );
        });
}

void BaseStairComponent::OnCollide(Actor* pOtherActor)
{
    if (!pOtherActor)
        return;

    if (auto* pInteract = pOtherActor->GetComponent<InteractionComponent>())
    {
        pInteract->SetCurrentStair(this);
        pInteract->SetPlayerActor(pOtherActor);
        BB_LOG(LogType::kMessage, "Player collided with a stair leading to ",
            m_data.targetLevelPath.c_str());

        if (m_data.autoUse)
        {
            BB_LOG(LogType::kMessage, "Auto-using stair to ", m_data.targetLevelPath.c_str());
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

    parser.NewChildVariable("MusicTrack", m_data.musicTrackPath);
    parser.NewChildVariable("SoundTrack", m_data.soundTrackPath);
    parser.NewChildVariable("Volume", m_data.volume);
}

void BaseStairComponent::Load(const XMLElementParser parser)
{
    TransitionData data;

    parser.GetChildVariable("TargetLevel", &data.targetLevelPath);
    parser.GetChildVariable("SpawnX", &data.newPosition.x);
    parser.GetChildVariable("SpawnY", &data.newPosition.y);
    parser.GetChildVariable("Fade", &data.fadeDuration);
    parser.GetChildVariable("AutoUse", &data.autoUse);
    parser.GetChildVariable("MusicTrack", &data.musicTrackPath);
    parser.GetChildVariable("SoundTrack", &data.soundTrackPath);
    parser.GetChildVariable("Volume", &data.volume);
    SetTransitionData(data);
}

void BaseStairComponent::OnStairUsed(Actor* pOtherActor)
{

    BlackBoxManager::Get()->m_pAudioManager->PlaySound(m_data.soundTrackPath.c_str(), m_data.volume);

    if (!pOtherActor)
        return;

    BB_LOG(LogType::kMessage, "Transitioning to: ", m_data.targetLevelPath );

    auto* pInteract = pOtherActor->GetComponent<InteractionComponent>();
    if (!pInteract) 
        return;

    auto* pTransform = pOtherActor->GetComponent<TransformComponent>();
    if (!pTransform) 
        return;

    auto* pManager = BlackBoxManager::Get();
    if (!pManager) 
        return;

    auto* pHUD = pOtherActor->GetComponent<PlayerStatsComponent>();
    if (!pHUD) 
        return;

    PlayerRuntimeStats savedStats
    {
        .HP = pHUD->m_playerHP,
        .MP = pHUD->m_playerMP,
        .Gold = pHUD->m_playerGold,
        .Energy = pHUD->m_playerEXP,
    };

    // Begin transition
    BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage( kLevelChanging, m_pOwner );
    pManager->m_pInputManager->StopAllInput();

    ScreenFader::FadeToBlack(m_data.fadeDuration);

    // Temporarily move and save player
    const auto currentPos = pTransform->m_position;
    pTransform->m_position = m_data.newPosition;
    pTransform->m_prevPosition = m_data.newPosition;

    pManager->m_pActorManager->SaveActor(pOtherActor, "PlayerActor", "../Assets/Actors/PlayerActor.xml");

    // Restore position before fade completes
    pTransform->m_position = currentPos;
    pHUD->m_playerHP = savedStats.HP;

    auto delayFunc = [pManager, data = m_data]() -> void
        {
            pManager->m_pInputManager->SwapInputToGame();
            pManager->m_pActorManager->LoadLevel(data.targetLevelPath.c_str());
            pManager->m_pInputManager->ResumeInput();

            BlackBoxManager::Get()->m_pAudioManager->SetMusicTrack(data.musicTrackPath.c_str());

            ScreenFader::FadeIn(data.fadeDuration);
            BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage( kLevelChangEnd, nullptr);
        };

    DelayedCallbackManager::AddCallback(delayFunc, std::chrono::milliseconds(int(m_data.fadeDuration * 1000)));
}
