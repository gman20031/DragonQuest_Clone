#include "Window.h"

#include "../BlackBoxManager.h"
#include "../System/Log.h"
#include "SDL.h"
#include "Renderer.h"
#include "Text Rendering/TextRenderer.h"

namespace BlackBoxEngine
{

    BB_Window::BB_Window(const char* title, int xPos, int yPos, int width, int height, BB_Window::WindowFlags flags)
        : m_pTitle(title)
        , m_xPos(xPos)
        , m_yPos(yPos)
        , m_width(width)
        , m_height(height)
        , m_windowFlags(flags)
    {

    }

    BB_Window::~BB_Window()
    {
        if (m_pSdlWindow)
            SDL_free(m_pSdlWindow);
        if (m_pRenderer)
            delete m_pRenderer;
        if (m_pTextRenderer)
            delete m_pTextRenderer;
    }

    int BB_Window::SetTitle(const char* title)
    {
        if (!SDL_SetWindowTitle(m_pSdlWindow, title))
        {
            BB_LOG(LogType::kError, SDL_GetError());
            return 1;
        }
        m_pTitle = title;
        return 0;
    }

    int BB_Window::SetDimensions(int width, int height)
    {
        if (!SDL_SetWindowSize(m_pSdlWindow, width, height))
        {
            BB_LOG(LogType::kError, SDL_GetError());
            return 1;
        }
        m_width = width;
        m_height = height;
        return 0;
    }

    int BB_Window::SetPosition(int x, int y)
    {
        if (!SDL_SetWindowPosition(m_pSdlWindow, x, y))
        {
            BB_LOG(LogType::kError, SDL_GetError());
            return 1;
        }
        m_width = x;
        m_height = y;
        return 0;
    }

    void BB_Window::NotifyWindowResized([[maybe_unused]]int newWidth, [[maybe_unused]] int newHeight)
    {
        //int w = 0, h = 0;
        //SDL_GetWindowSizeInPixels(m_pSdlWindow, &w, &h);
        SetDimensions(newWidth, newHeight);
        BlackBoxManager::Get()->m_pMessagingManager->EnqueueMessage("WindowSizeChanged", nullptr);
    }

    int BB_Window::StartWindow()
    {
        m_pSdlWindow = SDL_CreateWindow(m_pTitle, m_width, m_height, static_cast<uint64_t>(m_windowFlags) );
        if (!m_pSdlWindow)
        {
            BB_LOG(LogType::kError, SDL_GetError());
            return 1;
        }

        if (!SDL_SetWindowPosition(m_pSdlWindow, m_xPos, m_yPos))
        {
            BB_LOG(LogType::kError, SDL_GetError());
            return 1;
        }

        m_pRenderer = new BB_Renderer(this);
        if (!m_pRenderer)
        {
            BB_LOG(LogType::kError, "Renderer not Created");
            return 1;
        }

        m_pTextRenderer = new BB_TextRenderer(this);
        if (!m_pRenderer)
        {
            BB_LOG(LogType::kError, "Renderer not Created");
            return 1;
        }

        return 0;
    }

    int BB_Window::StopWindow()
    {
        SDL_free(m_pSdlWindow);
        delete m_pRenderer;
        m_isOpen = false;
        return 0;
    }

};


