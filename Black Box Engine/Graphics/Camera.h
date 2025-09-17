#pragma once

namespace BlackBoxEngine
{
    class BB_Window;
    struct BB_Rectangle;
    struct FVector2;

    class Camera
    {
    public:
        float m_screenXPos = 0;
        float m_screenYPos = 0;
        float m_width   = 256;
        float m_height  = 256;

        FVector2 GetZoomMults(const BB_Window* pWindow) const ;
        void OffestGameCoords(FVector2* pos, FVector2 zoom) const;
        void OffestGameCoords(float* x, float* y, FVector2 zoom) const;
        void ZoomDestinationRect(BB_Rectangle* pDest, FVector2 zoom) const;
    };

}

