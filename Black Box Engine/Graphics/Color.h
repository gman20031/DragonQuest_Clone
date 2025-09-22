#pragma once

#include <stdint.h>

namespace BlackBoxEngine
{
	struct ColorValue
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t a = 0;
	};

	namespace ColorPresets
	{
		inline static constexpr ColorValue white = {255,255,255,255};
		inline static constexpr ColorValue black = {0  ,0  ,0  ,255};

		inline static constexpr ColorValue red   = {255,0  ,0  ,255};
		inline static constexpr ColorValue green = {0  ,255,0  ,255};
		inline static constexpr ColorValue blue  = {0  ,0  ,255,255};

        inline static constexpr ColorValue purple = {50  ,0  ,255,255};
	}

}
