#pragma once

#include <BlackBoxManager.h>

class BlackBoxGame
{
    BlackBoxEngine::BlackBoxManager* m_pEngineManager;

private:
    inline static constexpr int kDefaultXPos = 100;
    inline static constexpr int kDefaultYPos = 100;
    inline static constexpr int kDefaultWidth = 1250;
    inline static constexpr int kDefaultHeight = 950;

    void InitGame();

public:
    BlackBoxGame();
    ~BlackBoxGame();

    void Launch();
};
