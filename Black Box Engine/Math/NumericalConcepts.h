#pragma once

#include <concepts>

namespace BlackBoxEngine
{
	template <typename data_t>
	concept NumericalValue = std::integral<data_t> || std::floating_point<data_t>;

	template <typename data_t>
	concept NonNumericalValue = !std::integral<data_t> && !std::floating_point<data_t>;
}