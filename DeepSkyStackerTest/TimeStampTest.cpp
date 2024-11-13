#include "stdafx.h"
#include "catch.h"

#include <chrono>
#include <format>


TEST_CASE("Chrono", "[Chrono]")
{
	SECTION("TimeStamp")
	{
		constexpr char const expected[] = "2024-11-13 13:42:07.615";  // hh:mm:ss.mmm
		constexpr long long milliseconds = 615LL + 7 * 1000LL + 42 * 60 * 1000LL + 13 * 60 * 60 * 1000LL +
			(2024 - 1970) * 365 * 24 * 60 * 60 * 1000LL + 11 * 30 * 24 * 60 * 60 * 1000LL + 13 * 24 * 60 * 60 * 1000LL
			- 13 * 24 * 60 * 60 * 1000LL + 27 * 1000LL; // These are the corrections

		char buffer[32] = { '\0' };

		const auto now = std::chrono::utc_clock::time_point{ std::chrono::milliseconds{ milliseconds } };
		std::format_to_n(buffer, std::size(buffer) - 1, "{:%F %T}", std::chrono::floor<std::chrono::milliseconds>(now));

		REQUIRE(std::string{ buffer } == std::string{ expected });
		// Test that the final string is terminated with a null character.
		REQUIRE(buffer[22] != '\0');
		REQUIRE(buffer[23] == '\0');
	}
}
