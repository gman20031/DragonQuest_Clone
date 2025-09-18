#pragma once

#include <concepts>

namespace BlackBoxEngine
{
	template <typename dataType>
	concept NumericalValue = std::integral<dataType> || std::floating_point<dataType>;

	template <typename dataType>
	concept NonNumericalValue = !std::integral<dataType> && !std::floating_point<dataType>;
}