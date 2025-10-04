#pragma once

namespace BlackBoxEngine
{
    class ScreenFader
    {
        friend class BlackBoxManager;
        static float m_remainingTime;

        inline static float m_fadePercent = 0.f;
        inline static float m_fadePerSecond = 0.f;
        inline static bool m_fadingIn = false;
        ScreenFader();

    public:
        static void FadeToBlack( float secondFadeOut );
        static void FadeIn( float secondFadeIn );
    private:
        static void Update( float deltaTime );
        static void Render();
    
    public:
        inline static bool m_fading = false;
    };


}
