#pragma once
#include "../Black Box Engine/Interface/InterfaceText.h"
#include "../Black Box Engine/Graphics/Text Rendering/Text.h"
#include <functional>
#include <string>

#include "../Black Box Engine/Graphics/RenderingStructs.h"

//class ScrollingTextNode : public BlackBoxEngine::InterfaceNode
//{
//public:
//    using Callback = std::function<void()>;
//
//    ScrollingTextNode(InterfaceNode* parent, const BB_FRectangle& rect, const std::string& text,
//        float charsPerSecond = 30.f, Callback onComplete = nullptr)
//        : InterfaceNode(parent), m_rect(rect), m_fullText(text),
//        m_charsPerSecond(charsPerSecond), m_onComplete(onComplete)
//    {
//        m_textNode = MakeChildNode<InterfaceText>("scrolling_line", rect, InterfaceText::Paremeters{
//            .pFontFile = "../Assets/Fonts/dragon-warrior-1.ttf",
//            .pText = "",
//            .textSize = 28.f,
//            .color = ColorPresets::white
//            });
//    }
//
//    void Update(float deltaTime) override
//    {
//        // Add new characters
//        if (m_currentLength < m_fullText.size())
//        {
//            m_accumulator += deltaTime * m_charsPerSecond;
//            int charsToShow = static_cast<int>(m_accumulator);
//            if (charsToShow > 0)
//            {
//                m_currentLength = std::min<int>(m_currentLength + charsToShow, m_fullText.size());
//                m_textNode->SetText(m_fullText.substr(0, m_currentLength).c_str());
//                m_accumulator -= charsToShow;
//            }
//        }
//        else if (!m_completed)
//        {
//            m_completed = true;
//            if (m_onComplete)
//                m_onComplete();
//        }
//
//        // Scroll text upward if needed
//        if (m_scrollUp)
//        {
//            m_rect.y -= m_scrollSpeed * deltaTime;
//            m_textNode->SetPosition(m_rect.x, m_rect.y);
//            if (m_rect.y + m_rect.h < 0) // completely off-screen
//                RemoveFromParent();
//        }
//    }
//
//    void StartScrollingUp(float speed)
//    {
//        m_scrollUp = true;
//        m_scrollSpeed = speed;
//    }
//
//private:
//    BB_FRectangle m_rect;
//    std::string m_fullText;
//    int m_currentLength = 0;
//    float m_charsPerSecond;
//    float m_accumulator = 0.f;
//    bool m_completed = false;
//    bool m_scrollUp = false;
//    float m_scrollSpeed = 0.f;
//    Callback m_onComplete;
//    InterfaceText* m_textNode = nullptr;
//};