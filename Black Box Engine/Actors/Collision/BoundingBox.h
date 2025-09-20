#pragma once

#include "../../Graphics/RenderingStructs.h"
#include "../../math/FVector2.h"

namespace BlackBoxEngine
{
    struct BoundingBox
    {
        BB_FRectangle m_rect;

        BoundingBox(BB_FRectangle rect);
        BoundingBox(float x, float y, float w, float h);

        float Right() const { return m_rect.x + m_rect.w; }
        float Bottom() const { return m_rect.y + m_rect.h; }
        float Top() const { return m_rect.y; }
        float Left() const { return m_rect.x; }

        /**
         * @brief Checks to see if the given position is FULLY contained within this bounding box
         * @param pos : Fvector position to compare against
         * @return : true if contained
         */
        bool Contains(FVector2 pos) const;
        /**
         * @brief Checks to see if the given box is FULLY contained within this bounding box
         * @param pos : Fvector position to compare against
         * @return : true if contained
         */
        bool Contains(const BoundingBox& box) const;
        /**
         * @brief Checks to see if the other bounding box intersects this at all.
         * @param box : to compare intersection
         * @return true if any intersect
         */
        bool Intersects(const BoundingBox& box) const;
        
        FVector2 Size() const { return { m_rect.w,m_rect.h }; }
        FVector2 Center() const { return { Right() / 2, Bottom() / 2 }; }
    };

}