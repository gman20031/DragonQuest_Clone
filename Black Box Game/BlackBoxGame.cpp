#include "BlackBoxGame.h"

using namespace BlackBoxEngine;

#include <cassert>

#include <Interface/InterfaceButton.h>
#include <Interface/InterfaceText.h>

#include "../Black Box Engine/Actors/EngineComponents/SpriteComponent.h"
#include "../Black Box Engine/Actors/EngineComponents/TransformComponent.h"

void ButtonOneCallback()
{
    BB_LOG(LogType::kMessage, "Button 1 clicked");
}
void ButtonTwoCallback()
{
    BB_LOG(LogType::kMessage, "Button 2 clicked");
}
void ButtonThreeCallback()
{
    BB_LOG(LogType::kMessage, "Button 3 clicked");
}
void ButtonFourCallback()
{
    BB_LOG(LogType::kMessage, "Button 4 clicked");
}

void BlackBoxGame::TestInterfaceStuff()
{
    using enum Direction;
    using namespace BlackBoxEngine;
    
    m_interfaceRoot.GetRoot()->SetOffset(20, 20);

    auto* pHighlighter = m_interfaceRoot.GetHighlight();
    pHighlighter->SetParmeters({
        .m_mode = InterfaceHighlighter::kModeUnderline | InterfaceHighlighter::kModeIcon,
        .m_pIconFile = "../Assets/UI/Icons/RightArrowTest.png",
        .m_iconOffset = {-9,0},
        .m_iconSize = {8,8},
        .m_lineWidth = 1,
        .m_underlineColor = ColorPresets::white,
    });

    BB_FRectangle buttonDimension{ 0,0, 64,8 };

    InterfaceButton::Parameters buttomParams{
        .usable = true,
        .mouseInteractable = false,
        .color =         ColorValue(255,0,0,0),
        .targetedColor = ColorValue(200,50,50,255),
        .interactColor = ColorValue(100,150,50,255)
    };

    ButtonCallbackFunctionPtr callbacks[] =
    {
        &ButtonOneCallback,
        &ButtonTwoCallback,
        &ButtonThreeCallback,
        &ButtonFourCallback
    };

    std::vector<InterfaceNode*> m_nodes;
    m_nodes.resize(4, nullptr);

    float yPad = 1;
    size_t buttonCount = std::size(callbacks);
    for (size_t i = 0; i < buttonCount; ++i)
    {
        std::string name = "button_" + std::to_string(i);
        buttomParams.callbackFunction = callbacks[i];
        buttonDimension.y = i * (buttonDimension.h + yPad);
        auto* pButton = m_interfaceRoot.AddNode<InterfaceButton>(name.c_str(), buttonDimension, buttomParams);
        m_nodes[i] = pButton;

        if (i == 0)
            continue;
        pButton->SetAdjacentNode(kUp, m_nodes[i - 1]);
        m_nodes[i - 1]->SetAdjacentNode(kDown, pButton);
    }
    m_nodes[3]->SetAdjacentNode(kDown, m_nodes[0]);
    m_nodes[0]->SetAdjacentNode(kUp, m_nodes[3]);

    buttonDimension.y = 0;

    InterfaceText::Parameters textParams{
        .pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf",
        .textSize = 12,
        .color = ColorPresets::white
    };

    textParams.pText = "First Button";
    m_nodes[0]->MakeChildNode<InterfaceText>("FirstButton Text", buttonDimension, textParams);

    textParams.pText = "Second Button";
    m_nodes[1]->MakeChildNode<InterfaceText>("SecondButton Text", buttonDimension, textParams);

    textParams.pText = "Third Button";
    m_nodes[2]->MakeChildNode<InterfaceText>("ThirdButton Text", buttonDimension, textParams);

    textParams.pText = "Fourth Button";
    m_nodes[3]->MakeChildNode<InterfaceText>("FourthButton Text", buttonDimension, textParams);

    pHighlighter->SetTarget(m_nodes[0]);

    m_interfaceRoot.SetCursorTarget(m_nodes[0]);
    m_interfaceRoot.AddToScreen();
    m_pEngineManager->m_pInputManager->SwapInputTargetToInterface(&m_interfaceRoot);
}

void BlackBoxGame::InitGame()
{
	BB_LOG(LogType::kMessage, "Game starting");
    m_pEngineManager->GetWindow()->SetWindowIcon("../Assets/Sprites/DragonQuestIcon.png");

    //TestInterfaceStuff();
    //m_pEngineManager->m_pActorManager->ClearLevel();
    m_pEngineManager->m_pActorManager->LoadLevel("../Assets/Levels/ExampleLevel.xml");
    //m_pEngineManager->m_pActorManager->LoadLevel("../Assets/Levels/Cave2Level.xml");

}

BlackBoxGame::BlackBoxGame()
    : m_pEngineManager(BlackBoxManager::NewSingleton())
    , m_pTileActorManager(new TileActorManager)
{
    m_pEngineManager->CreateWindow(
        "Dragon Quest Clone", kDefaultXPos, kDefaultYPos, kDefaultWidth, kDefaultHeight,
        BB_Window::kWindowFlag_Resizable
    );
    BB_LOG(LogType::kMessage, "Window Created");
}

BlackBoxGame::~BlackBoxGame()
{
    if (m_pEngineManager)
        m_pEngineManager->DeleteSingleton();
    if (m_pTileActorManager)
        delete m_pTileActorManager;
}

void BlackBoxGame::Launch()
{
    assert(m_pEngineManager);
    m_pEngineManager->InitEngine();
    InitGame();
    m_pTileActorManager->Start();
    m_pEngineManager->RunEngine();
}

BlackBoxGame* BlackBoxGame::NewSingleton()
{
    DeleteSingleton();
    s_pBlackBoxGame = new BlackBoxGame;
    return s_pBlackBoxGame;
}

BlackBoxGame* BlackBoxGame::Get()
{
    assert(s_pBlackBoxGame);
    return s_pBlackBoxGame;
}

void BlackBoxGame::DeleteSingleton()
{
    if (s_pBlackBoxGame)
    {
        delete s_pBlackBoxGame;
        s_pBlackBoxGame = nullptr;
    }
}
