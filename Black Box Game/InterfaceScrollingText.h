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
            float textSize = 28.f;
            ColorValue color = ColorPresets::white;
            float charsPerSecond = 30.f; // text typing speed
            float scrollSpeed = 50.f;     // pixels per second for smooth scroll
            int maxVisibleLines = 4;
            float charWidth = 5.f;       // fixed-width font assumption
            std::function<void()> onComplete = nullptr;
        };

        ScrollingTextBox(InterfaceNode* pParent, const char* pName, BB_FRectangle rect, const Params& params)
            : InterfaceNode(pParent, pName, rect), m_rect(rect), m_params(params)
        {
            constexpr float kPadding = 4.f;
            BB_FRectangle textRect{
                kPadding,
                kPadding,
                m_rect.w - 2 * kPadding,
                m_rect.h - 2 * kPadding
            };

            InterfaceText::Paremeters textParams{
                .pFontFile = m_params.fontFile,
                .pText = " ",
                .textSize = m_params.textSize,
                .color = m_params.color
            };

            m_textNode = new InterfaceText(this, "scrolling_text", textRect, textParams);
            m_childNodes.push_back(m_textNode);

            m_maxCharsPerLine = static_cast<int>(textRect.w / m_params.charWidth);
        }

        void SetText(const std::string& text)
        {
            m_fullText = text;
            m_currentChar = 0;
            m_accumulatedTime = 0.f;
            m_scrollOffset = 0.f;
            m_lines.clear();
            m_lines.push_back("");
            m_isComplete = false;

            UpdateDisplay();
        }

        void Update()
        {
            if (m_isComplete) return;

            

            float dt = static_cast<float>(BlackBoxManager::Get()->GetDeltaTime());

            // --- Reveal characters over time ---
            m_accumulatedTime += dt * m_params.charsPerSecond;
            int charsToReveal = static_cast<int>(m_accumulatedTime);
            if (charsToReveal > 0)
            {
                m_accumulatedTime -= charsToReveal;
                RevealCharacters(charsToReveal);
            }

            // --- Smooth scroll ---
            if (m_lines.size() > m_params.maxVisibleLines)
            {
                float targetOffset = (m_lines.size() - m_params.maxVisibleLines) * m_params.textSize;
                m_scrollOffset += m_params.scrollSpeed * dt;
                if (m_scrollOffset > targetOffset)
                    m_scrollOffset = targetOffset;

                UpdateDisplay();
            }

            // --- Completion check ---
            if (m_currentChar >= m_fullText.size())
            {
                m_isComplete = true;

                UpdateDisplay();

                if (!m_callbackFired && m_params.onComplete)
                {
                    m_callbackFired = true;
                    auto cb = m_params.onComplete;
                    m_params.onComplete = nullptr;
                    cb();
                }
            }
        }

    private:
        void RevealCharacters(int count)
        {
            for (int i = 0; i < count && m_currentChar < m_fullText.size(); ++i)
            {
                char c = m_fullText[m_currentChar++];

                if (c == '\n' || m_lines.back().size() >= m_maxCharsPerLine)
                    m_lines.push_back("");

                if (c != '\n')
                    m_lines.back() += c;

                // Do not erase lines immediately; scrolling handled by offset
            }

            UpdateDisplay();

            if (m_currentChar >= m_fullText.size())
            {
                m_isComplete = true;
                if (m_params.onComplete) m_params.onComplete();
            }
        }

        void UpdateDisplay()
        {
            if (!m_textNode || !m_textNode->GetText())
                return;

            constexpr float kPadding = 4.f;

            // Ensure we keep only the lines that fit in the box
            while (m_lines.size() > m_params.maxVisibleLines)
            {
                m_lines.erase(m_lines.begin()); // remove oldest line
            }

            // Build display string with horizontal centering
            std::string display;
            for (auto& line : m_lines)
            {
                display += line + "\n";
      
            }

            if (!display.empty() && display.back() == '\n')
                display.pop_back();

            // Smooth vertical offset for appearing characters
            float verticalOffset = kPadding;
            if (m_lines.size() >= m_params.maxVisibleLines)
            {
                float targetOffset = (m_lines.size() - m_params.maxVisibleLines) * m_params.textSize;
                m_scrollOffset += m_params.scrollSpeed * static_cast<float>(BlackBoxManager::Get()->GetDeltaTime());
                if (m_scrollOffset > targetOffset)
                    m_scrollOffset = targetOffset;

                verticalOffset -= m_scrollOffset;
            }

            m_textNode->SetOffset(0.f, verticalOffset);
            m_textNode->GetText()->SetString(display.c_str(), display.size());
        }

    private:
        BB_FRectangle m_rect;
        Params m_params;

        InterfaceText* m_textNode = nullptr;
        std::string m_fullText;
        std::vector<std::string> m_lines;

        int m_maxCharsPerLine = 40;
        float m_accumulatedTime = 0.f;
        size_t m_currentChar = 0;
        bool m_isComplete = false;
        bool m_callbackFired = false;
        float m_scrollOffset = 0.f; // current vertical scroll in pixels
    };

}