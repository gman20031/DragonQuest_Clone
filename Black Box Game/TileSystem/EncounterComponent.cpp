#include "EncounterComponent.h"
#include <Resources/ResourceManager.h>
#include <Actors/Actor.h>
#include <BlackBoxManager.h>
#include "../Black Box Game/EnemyComponent.h"

using namespace BlackBoxEngine;
//
//void EncounterComponent::StartEncounter()
//{
//    BB_LOG(LogType::kMessage, "EncounterComponent::StartEncounter() called!");
//
//    auto parser = m_xmlParser;
//    if (!parser) return;
//
//    auto enemiesNode = parser.GetChildElement("Enemies");
//    if (!enemiesNode)
//    {
//        BB_LOG(LogType::kWarning, "No enemies in this encounter!");
//        return;
//    }
//
//    // Fade to battle
//    ScreenFader::FadeToBlack(0.5f);
//
//    auto enemyNode = enemiesNode.GetChildElement("EnemyXML");
//    while (enemyNode)
//    {
//        const char* enemyPath = nullptr;
//        enemyNode.GetText(&enemyPath);
//
//        if (enemyPath)
//        {
//            auto enemyXML = ResourceManager::GetActorXMLData(enemyPath);
//            auto* enemyActor = BlackBoxManager::Get()->m_pActorManager->MakeActor(enemyXML);
//
//            if (auto* enemyComp = enemyActor->GetComponent<EnemyComponent>())
//            {
//                enemyComp->SetActiveInBattle(true);
//                BB_LOG(LogType::kMessage, "Enemy %s has appeared!", enemyComp->GetName().c_str());
//            }
//        }
//        enemyNode = enemyNode.GetSibling();
//    }
//
//    // Build combat UI
//    BuildCombatUI();
//
//    // Fade back in
//    ScreenFader::FadeIn(0.5f);
//
//}
//
//void EncounterComponent::BuildCombatUI()
//{
//    m_battleUI = UserInterface();
//
//    auto backgroundRect = BB_FRectangle{ 16, 120, 224, 40 };
//    auto* window = m_battleUI.CreateNode("BattleWindow", backgroundRect);
//    window->SetBackground(ColorPresets::black);
//    window->SetBorder(ColorPresets::white);
//
//    InterfaceText::Paremeters textParams;
//    textParams.pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf";
//    textParams.textSize = 16;
//    textParams.color = ColorPresets::white;
//    textParams.pText = "A Slime draws near!";
//
//    auto* textNode = m_battleUI.CreateTextNode("BattleText", window, textParams);
//}
