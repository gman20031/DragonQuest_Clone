#pragma once

#include <stdint.h>
#include "../System/StringHash.h"

namespace BlackBoxEngine
{

	struct BB_Point
	{
		float x;
		float y;
	};

    struct BB_IntPoint
    {
        float x;
        float y;
    };

	/**
     * @brief X
     * @brief Y
     * @brief W
     * @brief H
	 */
	struct BB_Rectangle
	{
		float x;
		float y;
		float w;
		float h;
	};

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