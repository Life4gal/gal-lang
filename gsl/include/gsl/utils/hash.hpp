#pragma once

#include <cstdint>

namespace gal::gsl::utils
{
	constexpr static std::uint64_t kGslHash64Killed = 1;

	constexpr auto hash64(const std::ranges::range auto& range) noexcept -> std::uint64_t
	{
		// FNV-1a hash. See: http://www.isthe.com/chongo/tech/comp/fnv/
		constexpr std::uint64_t hash_init{14695981039346656037ull};
		constexpr std::uint64_t hash_prime{1099511628211ull};

		auto hash = hash_init;

		for (const auto i: range)
		{
			hash ^= i;
			hash *= hash_prime;
		}

		if (hash <= kGslHash64Killed) { return hash_prime; }

		return hash;
	}
}
