#pragma once

#include <memory>
#include "RenderingStructs.h"

struct SDL_Window;
struct SDL_Surface;

namespace BlackBoxEngine
{
    class UserInterface;

	class BB_Window
	{
		friend class BB_Renderer;
        friend class BB_TextRenderer;
    public:
        // Copied from SDL_3 Window Flags -- https://wiki.libsdl.org/SDL3/SDL_WindowFlags
        using WindowFlags = uint64_t;
        static constexpr WindowFlags kWindowFlag_None               = 0;
        static constexpr WindowFlags kWindowFlag_FullScreen         = 0x00000000000001; //  window is in fullscreen mode
        static constexpr WindowFlags kWindowFlag_OpenGL             = 0x00000000000002; //  window usable with OpenGL context
        static constexpr WindowFlags kWindowFlag_Occluded           = 0x00000000000004; //  window is occluded
        static constexpr WindowFlags kWindowFlag_Hidden             = 0x00000000000008; //  window is neither mapped onto the desktop nor shown in the taskbar/dock/window list; SDL_ShowWindow() is required for it to become visible
        static constexpr WindowFlags kWindowFlag_Borderless         = 0x00000000000010; //  no window decoration
        static constexpr WindowFlags kWindowFlag_Resizable          = 0x00000000000020; //  window can be resized
        static constexpr WindowFlags kWindowFlag_Minimized          = 0x00000000000040; //  window is minimized
        static constexpr WindowFlags kWindowFlag_Maximized          = 0x00000000000080; //  window is maximized
        static constexpr WindowFlags kWindowFlag_MouseGrabbed       = 0x00000000000100; //  window has grabbed mouse input
        static constexpr WindowFlags kWindowFlag_InputFocus         = 0x00000000000200; //  window has input focus
        static constexpr WindowFlags kWindowFlag_MouseFocus         = 0x00000000000400; //  window has mouse focus
        static constexpr WindowFlags kWindowFlag_External           = 0x00000000000800; //  window not created by SDL
        static constexpr WindowFlags kWindowFlag_Modal              = 0x00000000001000; //  window is modal
        static constexpr WindowFlags kWindowFlag_HighPixelDensity   = 0x00000000002000; //  window uses high pixel density back buffer if possible
        static constexpr WindowFlags kWindowFlag_MouseCaptured      = 0x00000000004000; //  window has mouse captured (unrelated to MOUSE_GRABBED)
        static constexpr WindowFlags kWindowFlag_MouseRelative      = 0x00000000008000; //  window has relative mode enabled
        static constexpr WindowFlags kWindowFlag_AlwaysOnTop        = 0x00000000010000; //  window should always be above others
        static constexpr WindowFlags kWindowFlag_Utility            = 0x00000000020000; //  window should be treated as a utility window, not showing in the task m_bar and window list
        static constexpr WindowFlags kWindowFlag_Tooptip            = 0x00000000040000; //  window should be treated as a tooltip and does not get mouse or keyboard focus, requires a parent windo
        static constexpr WindowFlags kWindowFlag_PopupMenu          = 0x00000000080000; //  window should be treated as a popup menu, requires a parent window
        static constexpr WindowFlags kWindowFlag_KeyboardGrabbed    = 0x00000000100000; //  window has grabbed keyboard input
        static constexpr WindowFlags kWindowFlag_Vulkan             = 0x00000010000000; //  window usable for Vulkan surface
        static constexpr WindowFlags kWindowFlag_Metal              = 0x00000020000000; //  window usable for Metal view
        static constexpr WindowFlags kWindowFlag_Transparent        = 0x00000040000000; //  window with transparent buffer
        static constexpr WindowFlags kWindowFlag_NotFocusable       = 0x00000080000000; //  window not focusable
         
	private:           
        SDL_Window* m_pSdlWindow = nullptr;
        SDL_Surface* m_pIconSurface = nullptr;
		BB_Renderer* m_pRenderer = nullptr;
        BB_TextRenderer* m_pTextRenderer = nullptr;

		const char* m_pTitle = nullptr;
		int m_xPos = 0;
		int m_yPos = 0;
		int m_width = 100;
		int m_height = 100;

		int m_SdlErrorCode = 0;
        WindowFlags m_windowFlags = kWindowFlag_None;
		bool m_isOpen = true;

	public:
        BB_Window(const char* title, int xPos, int yPos, int width, int height, WindowFlags flags);
		~BB_Window();
		BB_Window(const BB_Window&) = delete;
		BB_Window& operator=(const BB_Window&) = delete;

		int SetTitle(const char* title);
        int SetDimensions(int width, int height);
        int SetPosition(int x, int y);
        bool SetWindowIcon(const char* pIconPath);

        const char* GetTitle() const { return m_pTitle; }
        BB_IntRectangle GetDimensions() const { return { m_xPos,m_yPos,m_width,m_height }; }
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        int GetXPos() const { return m_xPos; }
        int GetYPos() const { return m_xPos; }

        BB_Renderer* const GetRenderer() const { return m_pRenderer; }
        BB_TextRenderer* const GetTextRenderer() const { return m_pTextRenderer; }

        void NotifyWindowResized(int newWidth, int newHeight);

		int StartWindow();
		int StopWindow();
		bool IsOpen() const { return m_isOpen; }
	};


}
