#include "Camera.h"

#include "../BlackBoxManager.h"
#include "../Math/FVector2.h"
#include "Window.h"
#include "RenderingStructs.h"

namespace BlackBoxEngine
{
    ////////////////////////////////////////////////////////////////
    // Variable access/manip
    ////////////////////////////////////////////////////////////////
    void BB_Camera::SetCenterPoint(FVector2 centerPos)
    {
        m_centerPointCache = centerPos;
        m_screenInfoRect.x = centerPos.x - (m_screenInfoRect.w / 2);
        m_screenInfoRect.y = centerPos.y - (m_screenInfoRect.h / 2);
    }

    void BB_Camera::Resize(float width, float height)
    {
        m_screenInfoRect.w = width;
        m_screenInfoRect.h = height;
        m_screenInfoRect.x = m_centerPointCache.x - (m_screenInfoRect.w / 2);
        m_screenInfoRect.y = m_centerPointCache.y - (m_screenInfoRect.h / 2);
    }

    FVector2 BB_Camera::GetCenterPoint() const
    {
        return m_centerPointCache;
    }

    FVector2 BB_Camera::GetDimensions() const
    {
        return { m_screenInfoRect.w , m_screenInfoRect.h };
    }

    ////////////////////////////////////////////////////////////////
    // Screen offsetting
    ////////////////////////////////////////////////////////////////

    BB_Rectangle BB_Camera::ConvertToScreenPos(const BB_Rectangle gameRect) const
    {
        const auto& pWindow = BlackBoxManager::Get()->GetWindow();
        BB_Rectangle outputRect = gameRect;
        FVector2 zoom = GetXYZoom(pWindow);
        ConvertToScreenPos(&outputRect.x, &outputRect.y, zoom);
        ZoomDestinationRect(&outputRect, zoom);
        return outputRect;
    }

    FVector2 BB_Camera::GetXYZoom(const BB_Window* pWindow) const
    {
        float x = static_cast<float>(pWindow->GetWidth())  / m_screenInfoRect.w;
        float y = static_cast<float>(pWindow->GetHeight()) / m_screenInfoRect.h;
        return FVector2{ x,y };
    }

    FVector2 BB_Camera::GetXYZoom(const std::unique_ptr<BB_Window>& pWindow) const
    {
        float x = static_cast<float>(pWindow->GetWidth()) / m_screenInfoRect.w;
        float y = static_cast<float>(pWindow->GetHeight()) / m_screenInfoRect.h;
        return FVector2{ x,y };
    }

    void BB_Camera::ConvertToScreenPos(FVector2* pPos, FVector2* zoom) const
    {
        ConvertToScreenPos(&pPos->x, &pPos->y, *zoom);
    }

    void BB_Camera::ConvertToScreenPos(float* pX, float* pY, FVector2 zoom) const
    {
        *pX = (*pX * zoom.x) - (m_screenInfoRect.x * zoom.x);
        *pY = (*pY * zoom.y) - (m_screenInfoRect.y * zoom.y);
    }

    ////////////////////////////////////////////////////////////////
    // Statics Members
    ////////////////////////////////////////////////////////////////
    
    void BB_Camera::ZoomDestinationRect(BB_Rectangle* pDest, FVector2 zoom)
    {
        pDest->w *= zoom.x;
        pDest->h *= zoom.y;
    }   

}

