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
    void BB_Camera::SetCenterPoint( FVector2 centerPos )
    {
        m_centerPointCache = centerPos;
        m_screenInfoRect.x = centerPos.x - (m_screenInfoRect.w / 2);
        m_screenInfoRect.y = centerPos.y - (m_screenInfoRect.h / 2);
    }

    void BB_Camera::Resize( float width, float height )
    {
        m_screenInfoRect.w = width;
        m_screenInfoRect.h = height;
        m_screenInfoRect.x = m_centerPointCache.x - (m_screenInfoRect.w / 2);
        m_screenInfoRect.y = m_centerPointCache.y - (m_screenInfoRect.h / 2);
        m_screenZoom = GetCameraWindowZoom( BlackBoxManager::Get()->GetWindow() );
    }

    void BB_Camera::SetScreenDimensions( BB_FRectangle newDimensions )
    {
        m_screenInfoRect = newDimensions;
        m_centerPointCache.x = newDimensions.x + (newDimensions.w / 2);
        m_centerPointCache.y = newDimensions.y + (newDimensions.h / 2);
        m_screenZoom = GetCameraWindowZoom( BlackBoxManager::Get()->GetWindow() );
    }

    FVector2 BB_Camera::GetTopLeft() const
    {
        return FVector2( m_screenInfoRect.x, m_screenInfoRect.y );
    }

    FVector2 BB_Camera::GetCenterPoint() const
    {
        return m_centerPointCache;
    }

    FVector2 BB_Camera::GetDimensions() const
    {
        return {m_screenInfoRect.w , m_screenInfoRect.h};
    }

    ////////////////////////////////////////////////////////////////
    // Screen offsetting
    ////////////////////////////////////////////////////////////////

    BB_FRectangle BB_Camera::ConvertGameToScreen( const BB_FRectangle& gameRect ) const
    {
        BB_FRectangle outputRect = gameRect;
        outputRect.x -= m_screenInfoRect.x;
        outputRect.y -= m_screenInfoRect.y;
        return outputRect;
    }

    BB_FRectangle BB_Camera::ZoomScreenRect( const BB_FRectangle& inRect ) const
    {
        const auto& pWindow = BlackBoxManager::Get()->GetWindow();
        BB_FRectangle outputRect = inRect;
        FVector2 zoom = GetCameraWindowZoom( pWindow );
        outputRect.x *= zoom.x;
        outputRect.y *= zoom.y;
        outputRect.w *= zoom.x;
        outputRect.h *= zoom.y;
        return outputRect;
    }

    void BB_Camera::ShiftGamePositionToScreen( float* pX, float* pY ) const
    {
        auto zoom = GetCameraWindowZoom( BlackBoxManager::Get()->GetWindow() );
        *pX -= m_screenInfoRect.x;
        *pY -= m_screenInfoRect.y;
        *pX *= zoom.x;
        *pY *= zoom.y;
    }

    FVector2 BB_Camera::GetCameraWindowZoom( const BB_Window* pWindow ) const
    {
        float x = static_cast<float>(pWindow->GetWidth()) / m_screenInfoRect.w;
        float y = static_cast<float>(pWindow->GetHeight()) / m_screenInfoRect.h;
        return FVector2{x,y};
    }

    FVector2 BB_Camera::GetCameraWindowZoom( const std::unique_ptr<BB_Window>& pWindow ) const
    {
        float x = static_cast<float>(pWindow->GetWidth()) / m_screenInfoRect.w;
        float y = static_cast<float>(pWindow->GetHeight()) / m_screenInfoRect.h;
        return FVector2{x,y};
    }
}