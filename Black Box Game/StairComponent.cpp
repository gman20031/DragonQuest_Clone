#include "StairComponent.h"

#include <chrono>
#include <BlackBoxManager.h>
#include <Graphics/ScreenFader.h>
#include <Actors/EngineComponents/TransformComponent.h>
#include <System/Delay.h>

#include "BlackBoxGame.h"
#include "InteractionComponent.h"

using namespace BlackBoxEngine;

void CaveEntranceComponent::OnStairUsed([[maybe_unused]] Actor* pOtherActor)
{
	
}

void CaveEntranceComponent::OpenLevel(BlackBoxEngine::Actor* pOtherActor)
{
    BB_LOG(LogType::kMessage, "Entering the cave...");

    // check if player
    auto* pInteractComponent = pOtherActor->GetComponent<InteractionComponent>();
    if (!pInteractComponent)
        return;

    pInteractComponent->OnLevelTransitionStart();

    // player should have transform
    auto* pTransform = pOtherActor->GetComponent<TransformComponent>();
    if (!pTransform)
        return;

    auto* pManager = BlackBoxManager::Get();
    auto currentPos = pTransform->m_position;

    pTransform->m_position = { 16, 16 };

    pTransform->m_prevPosition = { 16, 16 };

    pManager->m_pActorManager->SaveActor(pOtherActor, "PlayerActor", "../Assets/Actors/PlayerActor.xml");
    pTransform->m_position = currentPos;

    // stop all input.
    pManager->m_pInputManager->StopAllInput();
    pInteractComponent->HideHUD();

    ScreenFader::FadeToBlack(1.f);

    auto delayFunc = [pManager, pOtherActor, pInteractComponent]()
        {
            //auto* pInteractComponent = pOtherActor->GetComponent<InteractionComponent>();
            //pInteractComponent->HideHUD();

            pManager->m_pInputManager->SwapInputToGame();
            pManager->m_pActorManager->LoadLevel("../Assets/Levels/Cave1Level.xml");
            pManager->m_pActorManager->Start();
            pManager->m_pInputManager->ResumeInput();

            ScreenFader::FadeIn(1.f);

            pInteractComponent->OnLevelTransitionEnd();

            return 0;
        };
    Delay(std::chrono::seconds(1), delayFunc);


  
}


void CaveEntranceComponent::OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{

	if (auto* player = pOtherActor->GetComponent<InteractionComponent>())
	{
		player->SetCurrentStair(this);
        player->SetPlayerActor(pOtherActor);

        BB_LOG(LogType::kMessage, "Player is now on cave entrance.");

        OpenLevel(pOtherActor);
	}

}



void StairDownComponent::OnStairUsed(BlackBoxEngine::Actor* pOtherActor)
{
    BB_LOG(LogType::kMessage, "Entering the cave...");

    // check if player
    auto* pInteractComponent = pOtherActor->GetComponent<InteractionComponent>();
    if (!pInteractComponent)
        return;

    pInteractComponent->OnLevelTransitionStart();

    // player should have transform
    auto* pTransform = pOtherActor->GetComponent<TransformComponent>();
    if (!pTransform)
        return;

    auto* pManager = BlackBoxManager::Get();
    auto currentPos = pTransform->m_position;


    pTransform->m_position = { 144, 160 };
    pTransform->m_prevPosition = { 144, 160 };

    pManager->m_pActorManager->SaveActor(pOtherActor, "PlayerActor", "../Assets/Actors/PlayerActor.xml");
    pTransform->m_position = currentPos;

    // stop all input.
    pManager->m_pInputManager->StopAllInput();
    pInteractComponent->HideHUD();

    ScreenFader::FadeToBlack(1.f);

    auto delayFunc = [pManager, pOtherActor, pInteractComponent]()
        {
           //auto* pInteractComponent = pOtherActor->GetComponent<InteractionComponent>();
           //pInteractComponent->HideHUD();

            pManager->m_pInputManager->SwapInputToGame();
            pManager->m_pActorManager->LoadLevel("../Assets/Levels/Cave2Level.xml");
            pManager->m_pActorManager->Start();
            pManager->m_pInputManager->ResumeInput();

            ScreenFader::FadeIn(1.f);
            pInteractComponent->OnLevelTransitionEnd();

            return 0;
        };
    Delay(std::chrono::seconds(1), delayFunc);
}

void StairDownComponent::OnCollide([[maybe_unused]] BlackBoxEngine::Actor* pOtherActor)
{
    if (auto* player = pOtherActor->GetComponent<InteractionComponent>())
    {
        player->SetCurrentStair(this);
        player->SetPlayerActor(pOtherActor);

        BB_LOG(LogType::kMessage, "Player is now on cave level 2 entrance.");
    }
}

void StairUpLevel1Component::OnStairUsed(BlackBoxEngine::Actor* pOtherActor)
{
    BB_LOG(LogType::kMessage, "Entering the cave...");

    // check if player
    auto* pInteractComponent = pOtherActor->GetComponent<InteractionComponent>();
    if (!pInteractComponent)
        return;

    pInteractComponent->OnLevelTransitionStart();
    // player should have transform
    auto* pTransform = pOtherActor->GetComponent<TransformComponent>();
    if (!pTransform)
        return;

    auto* pManager = BlackBoxManager::Get();
    auto currentPos = pTransform->m_position;


    pTransform->m_position = { 576, 320 };
    pTransform->m_prevPosition = { 576, 320 };

    pManager->m_pActorManager->SaveActor(pOtherActor, "PlayerActor", "../Assets/Actors/PlayerActor.xml");
    pTransform->m_position = currentPos;

    // stop all input.
    pManager->m_pInputManager->StopAllInput();
    pInteractComponent->HideHUD();

    ScreenFader::FadeToBlack(1.f);

    auto delayFunc = [pManager, pOtherActor, pInteractComponent]()
        {
            //auto* pInteractComponent = pOtherActor->GetComponent<InteractionComponent>();
            //pInteractComponent->HideHUD();

            pManager->m_pInputManager->SwapInputToGame();
            pManager->m_pActorManager->LoadLevel("../Assets/Levels/ExampleLevel.xml");

            pManager->m_pActorManager->Start();
            pManager->m_pInputManager->ResumeInput();

            ScreenFader::FadeIn(1.f);

            pInteractComponent->OnLevelTransitionEnd();
            return 0;
        };
    Delay(std::chrono::seconds(1), delayFunc);
}

void StairUpLevel1Component::OnCollide(BlackBoxEngine::Actor* pOtherActor)
{
    if (auto* player = pOtherActor->GetComponent<InteractionComponent>())
    {
        player->SetCurrentStair(this);
        player->SetPlayerActor(pOtherActor);

        BB_LOG(LogType::kMessage, "Player is now on stair of cave 1 entrance.");
    }

}

void StairUpLevel2Component::OnStairUsed(BlackBoxEngine::Actor* pOtherActor)
{
    BB_LOG(LogType::kMessage, "Entering the cave...");

    // check if player
    auto* pInteractComponent = pOtherActor->GetComponent<InteractionComponent>();
    if (!pInteractComponent)
        return;

    pInteractComponent->OnLevelTransitionStart();

    // player should have transform
    auto* pTransform = pOtherActor->GetComponent<TransformComponent>();
    if (!pTransform)
        return;

    auto* pManager = BlackBoxManager::Get();
    auto currentPos = pTransform->m_position;


    pTransform->m_position = { 160, 160 };

    pTransform->m_prevPosition = { 160, 160 };

    pManager->m_pActorManager->SaveActor(pOtherActor, "PlayerActor", "../Assets/Actors/PlayerActor.xml");
    pTransform->m_position = currentPos;

    // stop all input.
    pManager->m_pInputManager->StopAllInput();
    //pInteractComponent->HideHUD();
    ScreenFader::FadeToBlack(1.f);

    auto delayFunc = [pManager, pOtherActor, pInteractComponent]()
        {
            //auto* pInteractComponent = pOtherActor->GetComponent<InteractionComponent>();
            //pInteractComponent->HideHUD();

            pManager->m_pInputManager->SwapInputToGame();
            pManager->m_pActorManager->LoadLevel("../Assets/Levels/Cave1Level.xml");
            pManager->m_pActorManager->Start();
            pManager->m_pInputManager->ResumeInput();

            ScreenFader::FadeIn(1.f);

            pInteractComponent->OnLevelTransitionEnd();

            return 0;
        };
    Delay(std::chrono::seconds(1), delayFunc);
}

void StairUpLevel2Component::OnCollide(BlackBoxEngine::Actor* pOtherActor)
{
    if (auto* player = pOtherActor->GetComponent<InteractionComponent>())
    {
        player->SetCurrentStair(this);
        player->SetPlayerActor(pOtherActor);

        BB_LOG(LogType::kMessage, "Player is now on stair of cave 2 entrance.");
    }
}
