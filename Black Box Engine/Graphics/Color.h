#pragma once

#include <stdint.h>

namespace BlackBoxEngine
{
	struct ColorValue
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	namespace ColorPresets
	{
		inline static constexpr ColorValue white = {255,255,255,255};
		inline static constexpr ColorValue black = {0  ,0  ,0  ,255};

		inline static constexpr ColorValue red   = {255,0  ,0  ,255};
		inline static constexpr ColorValue green = {0  ,255,0  ,255};
		inline static constexpr ColorValue blue  = {0  ,0  ,255,255};
	}

}
