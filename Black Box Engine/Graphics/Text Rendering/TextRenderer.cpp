#include "TextRenderer.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "../../Resources/ResourceManager.h"
#include "../../BlackBoxManager.h"
#include "../../System/Log.h"
#include "../Renderer.h"
#include "Font.h"
#include "Text.h"

namespace BlackBoxEngine
{
    BB_TextRenderer::BB_TextRenderer(BB_Window* pWindow)
        :m_pAttachedWindow(pWindow)
    {
        m_pTtfEngine = TTF_CreateRendererTextEngine(pWindow->GetRenderer()->m_pSdlRenderer);
        m_pGameCamera = BlackBoxManager::Get()->m_pMainCamera;
    }

    BB_TextRenderer::~BB_TextRenderer()
    {
        TTF_DestroyRendererTextEngine(m_pTtfEngine);
    }

    BB_Renderer* BB_TextRenderer::GetStandardRenderer() const
    {
        return m_pAttachedWindow->GetRenderer();
    }

    bool BB_TextRenderer::DrawTextBlended(std::shared_ptr<BB_Font> pFont, const char* pText, size_t length, ColorValue foregroundColor, BB_FRectangle destination)
    {
        SDL_Surface* pSurface = TTF_RenderText_Blended(pFont->m_pTtfFont, pText, length, SDL_Color{foregroundColor.r, foregroundColor.g, foregroundColor.b, foregroundColor.a} );
        if (!pSurface)
        {
            BB_LOG(LogType::kError, "surface not created - ", SDL_GetError());
            return false;
        }

        const SDL_Rect destRect((int)destination.x, (int)destination.y, (int)destination.w, (int)destination.h);
        const SDL_PixelFormatDetails * pFormat = SDL_GetPixelFormatDetails(pSurface->format);
        bool good = SDL_FillSurfaceRect(pSurface, &destRect, SDL_MapRGBA(pFormat, nullptr, foregroundColor.r, foregroundColor.g, foregroundColor.b, foregroundColor.a));

        if (!good)
            BB_LOG(LogType::kError, "Error drawing surface : ", SDL_GetError());
        return good;
    }

    bool BB_TextRenderer::DrawTextSolid(std::shared_ptr<BB_Font> pFont, const char* pText, size_t length, ColorValue foregroundColor, BB_FRectangle destination)
    {
        SDL_Surface* pSurface = TTF_RenderText_Solid(pFont->m_pTtfFont, pText, length, SDL_Color{ foregroundColor.r, foregroundColor.g, foregroundColor.b, foregroundColor.a });
        if (!pSurface)
        {
            BB_LOG(LogType::kError, "surface not created - ", SDL_GetError());
            return false;
        }
        const SDL_Rect destRect((int)destination.x, (int)destination.y, (int)destination.w, (int)destination.h);
        const SDL_PixelFormatDetails * pFormat = SDL_GetPixelFormatDetails(pSurface->format);
        bool good = SDL_FillSurfaceRect(pSurface, &destRect, SDL_MapRGBA(pFormat, nullptr, foregroundColor.r, foregroundColor.g, foregroundColor.b, foregroundColor.a));

        if (!good)
            BB_LOG(LogType::kError, "Error drawing surface : ", SDL_GetError());
        return good;
    }

    bool BB_TextRenderer::DrawTextShaded(std::shared_ptr<BB_Font> pFont, const char* pText, size_t length, ColorValue foregroundColor, ColorValue backgroundColor, BB_FRectangle destination)
    {
        SDL_Surface* pSurface = TTF_RenderText_Shaded(pFont->m_pTtfFont, pText, length,
            SDL_Color{ foregroundColor.r, foregroundColor.g, foregroundColor.b, foregroundColor.a },
            SDL_Color{ backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a }
        );
        if (!pSurface)
        {
            BB_LOG(LogType::kError, "surface not created - ", SDL_GetError());
            return false;
        }
        const SDL_Rect destRect((int)destination.x, (int)destination.y, (int)destination.w, (int)destination.h);
        const SDL_PixelFormatDetails * pFormat = SDL_GetPixelFormatDetails(pSurface->format);
        bool good = SDL_FillSurfaceRect(pSurface, &destRect, SDL_MapRGBA(pFormat, nullptr, foregroundColor.r, foregroundColor.g, foregroundColor.b, foregroundColor.a));

        if (!good)
            BB_LOG(LogType::kError, "Error drawing surface : ", SDL_GetError());
        return good;
    }

    bool BB_TextRenderer::DrawTextLCD(std::shared_ptr<BB_Font> pFont, const char* pText, size_t length, ColorValue foregroundColor, ColorValue backgroundColor, BB_FRectangle destination)
    {
        SDL_Surface* pSurface = TTF_RenderText_LCD(pFont->m_pTtfFont, pText, length,
            SDL_Color{ foregroundColor.r, foregroundColor.g, foregroundColor.b, foregroundColor.a },
            SDL_Color{ backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a }
        );
        if (!pSurface)
        {
            BB_LOG(LogType::kError, "surface not created - ", SDL_GetError());
            return false;
        }
        const SDL_Rect destRect((int)destination.x, (int)destination.y, (int)destination.w, (int)destination.h);
        const SDL_PixelFormatDetails* pFormat = SDL_GetPixelFormatDetails(pSurface->format);
        bool good = SDL_FillSurfaceRect(pSurface, &destRect, SDL_MapRGBA(pFormat, nullptr, foregroundColor.r, foregroundColor.g, foregroundColor.b, foregroundColor.a));
    
        if (!good)
            BB_LOG(LogType::kError, "Error drawing surface : ", SDL_GetError());
        return good;
    }

    bool BB_TextRenderer::RenderText(std::shared_ptr<BB_Text> text, float x, float y)
    {
        auto zoom = m_pGameCamera->GetCameraWindowZoom(m_pAttachedWindow);
        x *= zoom.x;
        y *= zoom.y;
        const bool good = TTF_DrawRendererText(text->m_pTtfText, x, y);
        if (!good)
            BB_LOG(LogType::kError, "Error rendering pTtfText : ", SDL_GetError());
        return good;
    }

    void BB_TextRenderer::RenderTestAll(std::shared_ptr<BB_Text> pText)
    {
        TTF_Text* pTtfText = pText->m_pTtfText;
        std::shared_ptr<BB_Font> pBBFont = pText->GetFont();
        ColorValue color{0,0,0,0 };
        ColorValue bgColor{255,255,255,0 };
        TTF_GetTextColor(pTtfText, &color.r, &color.g, &color.b, &color.a);
        

        const char pTextBlended[] = { "TTF_RenderText_Blended" };
        const char pTextSolid[] =   { "TTF_RenderText_Solid" };
        const char pTextShaded[] =  { "TTF_RenderText_Shaded" };
        const char pTextLCD[] =     { "TTF_RenderText_LCD" };
        //const char pRenderText[] =  { "TTF_DrawRendererText" };

        BB_FRectangle destination = { 0,0,100,100 };

        DrawTextBlended(pBBFont, pTextBlended, std::size(pTextBlended), color, destination);
        destination.y += 100;

        DrawTextSolid(pBBFont, pTextSolid, std::size(pTextSolid), color, destination);
        destination.y += 100;

        DrawTextShaded(pBBFont, pTextShaded, std::size(pTextShaded), color, bgColor, destination);
        destination.y += 100;

        DrawTextLCD(pBBFont, pTextLCD, std::size(pTextLCD), color, bgColor, destination);
        destination.y += 100;

        RenderText(pText, destination.x, destination.y);
    }

}

