#pragma once

namespace BlackBoxEngine
{

	struct BB_Point
	{
		float x;
		float y;
	};

	struct BB_Rectangle
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