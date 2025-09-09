#pragma once

#include "RenderingStructs.h"
#include "Color.h"
#include "Window.h"
#include "Texture.h"

struct SDL_Renderer;

namespace BlackBoxEngine
{
    class BB_Renderer
    {
        friend class TextureFactory;
    private: // variables
        SDL_Renderer* m_pSdlRenderer = nullptr;
        const BB_Window* m_pAttachedWindow = nullptr;

        // default renderer background color
        inline static constexpr ColorValue kDefaultBackgroundColor = ColorPresets::black;
        inline static constexpr ColorValue kDefaultDrawColor = ColorPresets::white;

        // current draw color
        ColorValue m_currentBackgroundColor;
        ColorValue m_currentDrawColor;

    private: // functions
        bool SetSDLDrawColor(const ColorValue& newColor);

    public:
        BB_Renderer(BB_Window* pWindow);
        ~BB_Renderer();
        void Clear();
        void Present();

        bool SetDrawColor(const ColorValue& newDrawColor);
        bool SetBackgroundColor(const ColorValue& newBackgroundColor);

        // draw line
        bool DrawLine(BB_Point start, BB_Point end);

        // draw rectangle
        bool DrawRect(const BB_Rectangle& rec);
        bool DrawRectFilled(const BB_Rectangle& rec);
        bool DrawRect(const BB_Rectangle& rec , const ColorValue& color);
        bool DrawRectFilled(const BB_Rectangle& rec, const ColorValue& color);

        const char* GetErrorStr();
        // draw texture
        bool DrawTexture(
            BB_Texture* texture,
            const BB_Rectangle* source = nullptr,
            const BB_Rectangle* dest = nullptr,
            const double rot = 0,
            const BB_Point* center = nullptr,
            const BB_FlipVal& flip = BB_FlipVal::kNone
        );

        // draw presetSprite
    };

};