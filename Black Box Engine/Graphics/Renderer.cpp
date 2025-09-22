#include "Renderer.h"

#include <SDL3/SDL.h>
#include <assert.h>

#include "../System/Log.h"
#include "../BlackBoxManager.h"
#include "Text Rendering/TextRenderer.h"

//////////////////////////////////////////////////////////////////
///  Private Functions
//////////////////////////////////////////////////////////////////

/**
 * @brief Calls SDL_SetRenderDrawColor() with the inputed colorvalue
 * @param newColor 
 * @return Forwards return of SDL_SetRenderDrawColor()
 */

bool BlackBoxEngine::BB_Renderer::SetSDLDrawColor(const ColorValue& newColor)
{
	return SDL_SetRenderDrawColor(m_pSdlRenderer,
		newColor.r, newColor.g, newColor.b, newColor.a);
}


//////////////////////////////////////////////////////////////////
///  Public Functions
//////////////////////////////////////////////////////////////////

BlackBoxEngine::BB_Renderer::BB_Renderer(BB_Window* pWindow)	
	: m_pAttachedWindow(pWindow)
	, m_currentBackgroundColor(kDefaultBackgroundColor)
	, m_currentDrawColor(kDefaultDrawColor)
{
	assert(pWindow);
    
    m_pGameCamera = BlackBoxManager::Get()->m_pMainCamera;
	m_pSdlRenderer = SDL_CreateRenderer(pWindow->m_pSdlWindow, NULL);
	if (!m_pSdlRenderer)
		SimpleLog(SDL_GetError());

	SetDrawColor(kDefaultDrawColor);
}

BlackBoxEngine::BB_Renderer::~BB_Renderer()
{
	if (m_pSdlRenderer)
		SDL_free(m_pSdlRenderer);
}

/**
 * @brief Sets the draw color to background color, and fills the screen with that color.
 * Then swaps back to the normal set draw color.
 */
void BlackBoxEngine::BB_Renderer::Clear()
{
	SetSDLDrawColor(m_currentBackgroundColor);
	SDL_RenderClear(m_pSdlRenderer);
	SetSDLDrawColor(m_currentDrawColor);
}

void BlackBoxEngine::BB_Renderer::Present()
{
    if (!SDL_RenderPresent(m_pSdlRenderer))
        BB_LOG(LogType::kError, SDL_GetError());
}

bool BlackBoxEngine::BB_Renderer::SetDrawColor(const ColorValue& newDrawColor)
{
	m_currentDrawColor = newDrawColor;
	return SetSDLDrawColor(newDrawColor);
}

bool BlackBoxEngine::BB_Renderer::SetBackgroundColor(const ColorValue& newBackgroundColor)
{
	m_currentBackgroundColor = newBackgroundColor;
	return true;
}

const char* BlackBoxEngine::BB_Renderer::GetErrorStr()
{
    return SDL_GetError();
}

BlackBoxEngine::BB_TextRenderer* BlackBoxEngine::BB_Renderer::GetTextVariant() const
{
    return m_pAttachedWindow->GetTextRenderer();
}

//////////////////////////////////////////////////////////////////
/// Drawing Function
//////////////////////////////////////////////////////////////////

//////////////// Lines ////////////////

bool BlackBoxEngine::BB_Renderer::DrawLineScreen(BB_FPoint start, BB_FPoint end)
{
	return SDL_RenderLine(m_pSdlRenderer, start.x, start.y, end.x, end.y);
}

bool BlackBoxEngine::BB_Renderer::DrawLineGame(BB_FPoint start, BB_FPoint end)
{
    if (!m_pGameCamera)
        return false;

    FVector2 zoom = m_pGameCamera->GetXYZoom(m_pAttachedWindow);
    m_pGameCamera->ConvertToScreenPos(&start.x, &start.y, zoom);
    m_pGameCamera->ConvertToScreenPos(&end.x, &end.y, zoom);
    return SDL_RenderLine(m_pSdlRenderer, start.x, start.y, end.x, end.y);
}

//////////////// Rects ////////////////

bool BlackBoxEngine::BB_Renderer::DrawRectScreen(const BB_FRectangle& rec)
{
    SDL_FRect sdlRect{ rec.x, rec.y , rec.w, rec.h };
    return SDL_RenderRect(m_pSdlRenderer, &sdlRect);
}

bool BlackBoxEngine::BB_Renderer::DrawRectScreenFilled(const BB_FRectangle& rec)
{
    SDL_FRect sdlRect{ rec.x, rec.y , rec.w, rec.h };
    return SDL_RenderFillRect(m_pSdlRenderer, &sdlRect);
}

bool BlackBoxEngine::BB_Renderer::DrawRectScreen(const BB_FRectangle& rec, const ColorValue& color)
{
    auto startColor = m_currentDrawColor;
    SetDrawColor(color);
    SDL_FRect sdlRect{ rec.x, rec.y , rec.w, rec.h };
    bool good = SDL_RenderRect(m_pSdlRenderer, &sdlRect);
    SetDrawColor(startColor);
    return good;
}

bool BlackBoxEngine::BB_Renderer::DrawRectScreenFilled(const BB_FRectangle& rec, const ColorValue& color)
{
    auto startColor = m_currentDrawColor;
    SetDrawColor(color);
    SDL_FRect sdlRect{ rec.x, rec.y , rec.w, rec.h };
    bool good = SDL_RenderFillRect(m_pSdlRenderer, &sdlRect);
    SetDrawColor(startColor);
    return good;
}

bool BlackBoxEngine::BB_Renderer::DrawRectGame(const BB_FRectangle& rec)
{
    if (!m_pGameCamera)
        return false;
    const BB_FRectangle newRect = m_pGameCamera->ConvertToScreenPos(rec);
    return DrawRectScreen(newRect);
}

bool BlackBoxEngine::BB_Renderer::DrawRectGameFilled(const BB_FRectangle& rec)
{
    if (!m_pGameCamera)
        return false;
    const BB_FRectangle newRect = m_pGameCamera->ConvertToScreenPos(rec);
    return DrawRectScreenFilled(newRect);
}

bool BlackBoxEngine::BB_Renderer::DrawRectGame(const BB_FRectangle& rec, const ColorValue& color)
{
    if (!m_pGameCamera)
        return false;
    const BB_FRectangle newRect = m_pGameCamera->ConvertToScreenPos(rec);
    return DrawRectScreen(newRect, color);
}

bool BlackBoxEngine::BB_Renderer::DrawRectGameFilled(const BB_FRectangle& rec, const ColorValue& color)
{
    if (!m_pGameCamera)
        return false;
    const BB_FRectangle newRect = m_pGameCamera->ConvertToScreenPos(rec);
    return DrawRectScreenFilled(newRect, color);
}

//////////////////////////////////////////////////////////////////
/// Rendering Textures
//////////////////////////////////////////////////////////////////

bool BlackBoxEngine::BB_Renderer::DrawTextureScreen(
    const BB_Texture* texture,
    const BB_FRectangle* pSource,
    const BB_FRectangle* pDest,
    const double rot,
    const BB_FPoint* pCenter,
    const BB_FlipVal& flip
)
{
    const SDL_FRect* pSdlSource = (const SDL_FRect*)(pSource);       // tested, this is faster
    const SDL_FRect* pSdlDest = (const SDL_FRect*)(pDest);           // than doing static_cast
    const SDL_FPoint* pSdlCenter = (const SDL_FPoint*)(pCenter);     // 
    const SDL_FlipMode sdlFlip = static_cast<SDL_FlipMode>(flip);   // my code is copying Sdl, so I guarntee this to work

    assert(m_pSdlRenderer);

    return SDL_RenderTextureRotated(
        m_pSdlRenderer,
        texture->m_pSdlTexture,
        pSdlSource,
        pSdlDest,
        rot,
        pSdlCenter,
        sdlFlip
    );
}

bool BlackBoxEngine::BB_Renderer::DrawTextureScreenColored(
    BB_Texture* texture,
    const BB_FRectangle* source,
    const BB_FRectangle* dest,
    const ColorValue* pcolor,
    const double rot,
    const BB_FPoint* center,
    const BB_FlipVal& flip)
{
    ColorValue preColor;
    texture->GetColorMod(&preColor);
    texture->SetColorMod(*pcolor);
    bool good = DrawTextureScreen(texture, source, dest, rot, center, flip);
    texture->SetColorMod(preColor);
    return good;
}

bool BlackBoxEngine::BB_Renderer::DrawTextureGame(
    const BB_Texture* texture,
    const BB_FRectangle* pSource,
    const BB_FRectangle* pDest,
    const double rot,
    const BB_FPoint* pCenter,
    const BB_FlipVal& flip
)
{
    if (!m_pGameCamera)
        return false;
    
    const SDL_FRect* pSdlSource = (const SDL_FRect*)(pSource);       // tested, this is faster
    const SDL_FPoint* pSdlCenter = (const SDL_FPoint*)(pCenter);     // than doing static_cast 
    const SDL_FlipMode sdlFlip = static_cast<SDL_FlipMode>(flip);    // my code is copying Sdl, this is private code, so I can guarntee this to work

    const SDL_FRect* pSdlDest = nullptr; 
    BB_FRectangle gamePosRect{ 0,0,0,0 };
    if (pDest)
    {
        gamePosRect = m_pGameCamera->ConvertToScreenPos(*pDest);
        pSdlDest = (const SDL_FRect*)(&gamePosRect);
    }
    else
        pSdlDest = (const SDL_FRect*)(pDest);

    assert(m_pSdlRenderer);

    return SDL_RenderTextureRotated(
        m_pSdlRenderer,
        texture->m_pSdlTexture,
        pSdlSource,
        pSdlDest,
        rot,
        pSdlCenter,
        sdlFlip
    );
}

bool BlackBoxEngine::BB_Renderer::DrawTextureGameColored(
    BB_Texture* texture,
    const BB_FRectangle* source,
    const BB_FRectangle* dest,
    const ColorValue* pcolor,
    const double rot,
    const BB_FPoint* center,
    const BB_FlipVal& flip)
{
    ColorValue preColor;
    texture->GetColorMod(&preColor);
    texture->SetColorMod(*pcolor);
    bool good = DrawTextureScreen(texture, source, dest, rot, center, flip);
    texture->SetColorMod(preColor);
    return good;
}
