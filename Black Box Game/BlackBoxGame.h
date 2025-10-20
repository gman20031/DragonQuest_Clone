#pragma once

#include <BlackBoxManager.h>

#include "StartGameHandler.h"
#include "TileSystem/TileActorManager.h"

class BlackBoxGame
{
    BlackBoxEngine::BlackBoxManager* m_pEngineManager;

private:
    inline static BlackBoxGame* s_pBlackBoxGame = nullptr;
    
    GameStarter m_gameStarter;

    TileActorManager* m_pTileActorManager = nullptr;

    inline static constexpr int kDefaultXPos = 100;
    inline static constexpr int kDefaultYPos = 100;
    inline static constexpr int kDefaultWidth = (256 * 3);
    inline static constexpr int kDefaultHeight = (256 * 3);

public:
    static BlackBoxGame* NewSingleton();
    static BlackBoxGame* Get();
    static void DeleteSingleton();
    
    TileActorManager* GetTileActorManager() const { return m_pTileActorManager; }

    void Launch();
    void GameUpdate();

    const GameStarter& GetGameStarter() const { return m_gameStarter; }
private:
    void InitGame();

    BlackBoxGame();
    ~BlackBoxGame();
};
