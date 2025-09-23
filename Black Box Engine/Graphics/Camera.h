#pragma once

#include "RenderingStructs.h"
#include "../Math/FVector2.h"

namespace BlackBoxEngine
{
    class BB_Window;
    struct FVector2;

    class BB_Camera
    {
        static constexpr float kDefaultWidth = 256;
        static constexpr float kDefaultHeight = 256;

        BB_FRectangle m_screenInfoRect = { 0,0, kDefaultWidth, kDefaultHeight };
        FVector2 m_centerPointCache   = { kDefaultWidth / 2 , kDefaultHeight / 2 };
        FVector2 m_screenZoom{ 1,1 };
    public:
        void SetCenterPoint(FVector2 centerPos);
        /**
         * @brief Maintains the centerpoint when resizing.
         * @brief The size of the camera is indepenant from the size of the screen.
         */
        void Resize(float width, float height);
        void SetScreenDimensions(BB_FRectangle newDimensions);
        
        FVector2 GetCenterPoint() const;
        FVector2 GetDimensions() const;

        BB_FRectangle ConvertGameToScreen(const BB_FRectangle& gameRect) const;
        BB_FRectangle OffsetScreenFromCamera(const BB_FRectangle& gameRect) const;

        void ShiftGamePositionToScreen(FVector2* pPos, FVector2* zoom) const;
        void ShiftGamePositionToScreen(float* pX, float* pY, FVector2 zoom) const;
        FVector2 GetCameraWindowZoom(const BB_Window* pWindow) const;
        FVector2 GetCameraWindowZoom(const std::unique_ptr<BB_Window>& pWindow) const ;

        static void ScaleDestinationRectToScreen(BB_FRectangle* pDest, FVector2 zoom);
        static void ScaleDestinationRectToScreen(float* pW, float* pH, FVector2 zoom);
        static void OffsetDestinationToCamera(BB_FRectangle* pDest, FVector2 zoom);
        static void OffsetDestinationToCamera(float* pX, float* pY, FVector2 zoom);
    };

}

