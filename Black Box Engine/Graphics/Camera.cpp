#include "Camera.h"

#include "../Math/FVector2.h"
#include "Window.h"
#include "RenderingStructs.h"
namespace BlackBoxEngine
{
    FVector2 Camera::GetZoomMults(const BB_Window* pWindow) const
    {
        float x = static_cast<float>(pWindow->GetWidth()) / m_width;
        float y = static_cast<float>(pWindow->GetHeight()) / m_height;
        return FVector2{ x,y };
    }

    void Camera::OffestGameCoords(FVector2* pos, FVector2 zoom) const
    {
        OffestGameCoords(&pos->x, &pos->y, zoom);
    }

    void Camera::OffestGameCoords(float* x, float* y, FVector2 zoom) const
    {
        *x = (*x * zoom.x) + (m_screenXPos * zoom.x);
        *y = (*y * zoom.y) + (m_screenYPos * zoom.y);
    }

    void Camera::ZoomDestinationRect(BB_Rectangle* pDest, FVector2 zoom) const
    {
        pDest->w *= zoom.x;
        pDest->h *= zoom.y;
    }
    

}

