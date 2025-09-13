#pragma once

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
        float x;
        float y;
        float w;
        float h;
    };

	enum class BB_FlipVal
	{
		kNone,
		kHorizontal,
		kVericle,
	};

}