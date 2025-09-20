#pragma once

#include <stdint.h>
#include "../System/StringHash.h"

namespace BlackBoxEngine
{
    template <std::unsigned_integral Type>
    struct BB_TPoint
    {
        Type x;
        Type y;
    };

    struct BB_FPoint
    {
        float x;
        float y;
    };

    struct BB_IntPoint
    {
        int x;
        int y;
    };

    /**
     * @brief X
     * @brief Y
     * @brief W
     * @brief H
     */
    template <std::unsigned_integral Type>
    struct BB_TRectangle
    {
        Type x;
        Type y;
        Type w;
        Type h;
    };

    /**
     * @brief X
     * @brief Y
     * @brief W
     * @brief H
     */
    struct BB_FRectangle
    {
        float x;
        float y;
        float w;
        float h;
    };

    /**
     * @brief X
     * @brief Y
     * @brief W
     * @brief H
     */
    struct BB_IntRectangle
    {
        int x;
        int y;
        int w;
        int h;
    };

    enum class BB_FlipVal
    {
        kNone,
        kHorizontal,
        kVericle,
    };

    enum class BB_AnchorPoint
    {
        kTopLeft,
        kTopMiddle,
        kTopRight,
        kCenterLeft,
        kCenterTrue,
        kCenterRight,
        kBotLeft,
        kBotmiddle,
        kBotRight,
        kCount
    };

    namespace StructHelpers
    {
        inline static const char* s_gAnchorPointToString[] =
        {
            "kTopLeft",
            "kTopMiddle",
            "kTopRight",
            "kCenterLeft",
            "kCenterTrue",
            "kCenterRight",
            "kBotLeft",
            "kBotmiddle",
            "kBotRight"
        };

        inline static constexpr uint32_t s_gAnchorpointStringHash[] =
        {
            StringHash("kTopLeft"),
            StringHash("kTopMiddle"),
            StringHash("kTopRight"),
            StringHash("kCenterLeft"),
            StringHash("kCenterTrue"),
            StringHash("kCenterRight"),
            StringHash("kBotLeft"),
            StringHash("kBotmiddle"),
            StringHash("kBotRight")
        };

    }

}