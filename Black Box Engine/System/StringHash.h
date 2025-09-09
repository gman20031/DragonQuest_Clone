#pragma once
#include <string>

namespace BlackBoxEngine
{
	// from eastl string.h 
	inline constexpr uint32_t StringHash(const char* cString)
	{
        constexpr uint32_t kPrime = 16777619U;
        constexpr uint32_t kOffset = 2166136261U;
		unsigned int character, result = kOffset; // They implement an FNV-like string hash.
		while ((character = *(cString++)) != '\0')
			result = (result * kPrime) ^ character;
		return (uint32_t)result;
	}

}//blackBoxEngine