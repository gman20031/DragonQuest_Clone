#include "BlackBoxGame.h"

using namespace BlackBoxEngine;

#include <cassert>

#include <Interface/InterfaceButton.h>
#include <Interface/InterfaceText.h>
#include <Interface/InterfaceTexture.h>

#include "InteractionComponent.h"
#include <Actors/EngineComponents/SpriteComponent.h>

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
    pHighlighter->SetParameters( {
        .mode = InterfaceHighlighter::kModeIcon,
        .pSpriteFile = "../Assets/UI/Icons/IconSpriteFile.xml",
        .iconOffset{-5, -2},
        .iconSize{ 4,7 }
    });

    //m_interfaceRoot.AddNode<InterfaceTexture>( "TextureTest", {50,0,16,16}, InterfaceTexture::TextureInfo{
    //    .pTextureFile = "../Assets/Sprites/Player/Fixed_OffsetCharacterSheet.png",
    //    .spriteDimensions = {16,16},
    //    .useFullImage = false,
    //    .animate = true,
    //    .spriteXCount = 8,
    //    .spriteYCount = 1,
    //    .spriteXPad = 0,
    //    .spriteYPad = 0,
    //    .animationStartIndex = 0,
    //    .animationEndIndex = 7,
    //    .spriteSheetIndex = 0,
    //    .framesPerSecond = 2,
    //    .repeat = true
    //} );

    BB_FRectangle buttonDimension{ 0,0, 64, 7 };

    InterfaceButton::ButtonParams buttomParams{
        .usable = true,
        .color =         ColorValue(0,0,0,0),
        .targetedColor = ColorValue(0,0,0,0),
        .interactColor = ColorValue(0,0,0,0)
    };

    ButtonCallbackFunctionPtr callbacks[] =
    {
        &ButtonOneCallback,
        &ButtonTwoCallback,
        &ButtonThreeCallback,
        &ButtonFourCallback
    };

    InterfaceNode* m_nodes[4] = {};

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
    buttonDimension.x = 0;

    InterfaceText::Paremeters textParams{
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

    m_interfaceRoot.SetCursorTarget(m_nodes[0]); // this will crash if you forget

    m_interfaceRoot.AddToScreen();
    m_pEngineManager->m_pInputManager->SwapInputTargetToInterface(&m_interfaceRoot);
}

void BlackBoxGame::InitGame()
{
	BB_LOG(LogType::kMessage, "Game starting");
    m_pEngineManager->GetWindow()->SetWindowIcon("../Assets/Sprites/DragonQuestIcon.png");

    TestInterfaceStuff();
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
