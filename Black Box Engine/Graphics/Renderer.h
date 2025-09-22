#pragma once

#include "RenderingStructs.h"
#include "Color.h"
#include "Window.h"
#include "Texture.h"
#include "Camera.h"

struct SDL_Renderer;

namespace BlackBoxEngine
{
    class BB_TextRenderer;

    class BB_Renderer
    {
        friend class TextureFactory;
        friend class BB_TextRenderer;
    private: // variables
        SDL_Renderer* m_pSdlRenderer = nullptr;
        const BB_Window* m_pAttachedWindow = nullptr;
        const BB_Camera* m_pGameCamera = nullptr;

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
        const char* GetErrorStr();

        BB_TextRenderer* GetTextVariant() const;

        bool SetDrawColor(const ColorValue& newDrawColor);
        bool SetBackgroundColor(const ColorValue& newBackgroundColor);

        // draw line
        bool DrawLineScreen(BB_FPoint start, BB_FPoint end);
        bool DrawLineGame(BB_FPoint start, BB_FPoint end);

        // draw rectangle
        bool DrawRectScreen(const BB_FRectangle& rec);
        bool DrawRectScreenFilled(const BB_FRectangle& rec);
        bool DrawRectScreen(const BB_FRectangle& rec , const ColorValue& color);
        bool DrawRectScreenFilled(const BB_FRectangle& rec, const ColorValue& color);

        bool DrawRectGame(const BB_FRectangle& rec);
        bool DrawRectGameFilled(const BB_FRectangle& rec);
        bool DrawRectGame(const BB_FRectangle& rec, const ColorValue& color);
        bool DrawRectGameFilled(const BB_FRectangle& rec, const ColorValue& color);

        // draw texture
        
        /**
         * @brief Draws a texture to the screen, using the coordinates based on the screen
         * @param texture The texture you wish to draw
         * @param source Where on the texture the render should source from, for sprite sheets, null for all
         * @param dest Where on the screen you want to render to
         * @param rot Rotation if you want any
         * @param center The center of the rotation.
         * @param flip Flip the image
         * @return 
         */
        bool DrawTextureScreen(
            const BB_Texture* texture,
            const BB_FRectangle* source = nullptr,
            const BB_FRectangle* dest = nullptr,
            const double rot = 0,
            const BB_FPoint* center = nullptr,
            const BB_FlipVal& flip = BB_FlipVal::kNone
        );
        /**
         * @brief Draws a texture to the screen, using the coordinates based on the screen
         * @param texture The texture you wish to draw
         * @param source Where on the texture the render should source from, for sprite sheets, null for all
         * @param dest Where on the screen you want to render to
         * @param rot Rotation if you want any
         * @param center The center of the rotation.
         * @param flip Flip the image
         * @return 
         */
        bool DrawTextureScreenColored(
            BB_Texture* texture,
            const BB_FRectangle* source = nullptr,
            const BB_FRectangle* dest = nullptr,
            const ColorValue* pcolor = nullptr,
            const double rot = 0,
            const BB_FPoint* center = nullptr,
            const BB_FlipVal& flip = BB_FlipVal::kNone
        );

        /**
         * @brief Draws a texture to the screen, using the the game's coordinates and offset by the camera's position
         * @param texture The texture you wish to draw
         * @param source Where on the texture the render should source from, for sprite sheets, null for all
         * @param dest Where in the game you wish to render too
         * @param rot Rotation if you want any
         * @param center The center of the rotation.
         * @param flip Flip the image
         * @return
         */
        bool DrawTextureGame(
            const BB_Texture* texture,
            const BB_FRectangle* source = nullptr,
            const BB_FRectangle* dest = nullptr,
            const double rot = 0,
            const BB_FPoint* center = nullptr,
            const BB_FlipVal& flip = BB_FlipVal::kNone
        );
        /**
         * @brief Draws a texture to the screen, using the coordinates based on the screen
         * @param texture The texture you wish to draw
         * @param source Where on the texture the render should source from, for sprite sheets, null for all
         * @param dest Where on the screen you want to render to
         * @param rot Rotation if you want any
         * @param center The center of the rotation.
         * @param flip Flip the image
         * @return 
         */
        bool DrawTextureGameColored(
            BB_Texture* texture,
            const BB_FRectangle* source = nullptr,
            const BB_FRectangle* dest = nullptr,
            const ColorValue* pcolor = nullptr,
            const double rot = 0,
            const BB_FPoint* center = nullptr,
            const BB_FlipVal& flip = BB_FlipVal::kNone
        );
    };

};