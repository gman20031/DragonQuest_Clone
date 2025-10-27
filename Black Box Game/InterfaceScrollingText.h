#pragma once
#include "../Black Box Engine/Interface/UserInterface.h"
#include "../Black Box Engine/Interface/InterfaceText.h"
#include "../Black Box Engine/Graphics/RenderingStructs.h"
#include "../Black Box Engine/Graphics/Text Rendering/Text.h"

#include <string>
#include <vector>
#include <functional>
#include <BlackBoxManager.h>

namespace BlackBoxEngine
{
    class ScrollingTextBox : public BlackBoxEngine::InterfaceNode
    {
    public:

        bool IsAnimating() const { return !m_isComplete; }

        struct Params
        {
            const char* fontFile = "../Assets/Fonts/dragon-warrior-1.ttf";
            float textSize = 26.f;
            ColorValue color = ColorPresets::white;
            float charsPerSecond = 30.f; // text typing speed
            float scrollSpeed = 50.f;     // pixels per second for smooth scroll
            float charWidth = 5.f;       // fixed-width font assumption
            std::function<void()> onComplete = nullptr;
        };

    private:
        Params m_params;

        InterfaceText* m_textNode = nullptr;
        std::string m_fullText;
        std::vector<std::string> m_lines;

        float m_accumulatedTime = 0.f;
        size_t m_currentChar = 0;
        bool m_isComplete = true;
        float m_scrollOffset = 0.f; // current vertical scroll in pixels

    public:
        ScrollingTextBox( InterfaceNode* pParent, const char* pName, BB_FRectangle rect, const Params& params );
        void SetText( const std::string& text , std::function<void()> onComplete = nullptr );

    protected:
        virtual void UpdateThis() override;

    private:
        void RevealCharacters( int count );
        void UpdateDisplay();

    };

}