#include "ScreenFader.h"

#include "../BlackBoxManager.h"

namespace BlackBoxEngine
{
    void ScreenFader::FadeToBlack( float secondFadeOut )
    {
        m_fading = true;
        m_fadePercent = 0.f;
        m_fadePerSecond =  1 / secondFadeOut;
        m_fadingIn = false;
    }

    void ScreenFader::FadeIn( float secondFadeIn )
    {
        m_fading = true;
        m_fadePercent = 1.f;
        m_fadePerSecond =  - 1 / secondFadeIn;
        m_fadingIn = true;
    }

    void ScreenFader::Update( float deltaTime )
    {
        m_fadePercent += m_fadePerSecond * deltaTime;
        if ( !m_fadingIn && m_fadePercent >= 1 )
            m_fading = false;
        if ( m_fading && m_fadePercent <= 0 )
            m_fading = false;
    }

    void ScreenFader::Render()
    {
        BB_FRectangle rect{};
        rect.w = (float)BlackBoxManager::Get()->GetWindow()->GetWidth();
        rect.h = (float)BlackBoxManager::Get()->GetWindow()->GetHeight();
        rect.x = 0;
        rect.y = 0;
        ColorValue color = {0,0,0,255};
        color.a = static_cast<uint8_t>(color.a * m_fadePercent);
        BlackBoxManager::Get()->GetWindow()->GetRenderer()->DrawRectScreenFilled( rect,color );
    }

}
