#pragma once

#include <eve/wide.hpp>

namespace gal::gsl::boost
{
	namespace vec = eve;

	// ReSharper disable once CppInconsistentNaming
	using vec2f = vec::wide<float, vec::fixed<2>>;
	// ReSharper disable once CppInconsistentNaming
	using vec3f = vec::wide<float, vec::fixed<3>>;
	// ReSharper disable once CppInconsistentNaming
	using vec4f = vec::wide<float, vec::fixed<4>>;

	// ReSharper disable once CppInconsistentNaming
	using vec2i = vec::wide<std::int32_t, eve::fixed<2>>;
	// ReSharper disable once CppInconsistentNaming
	using vec3i = vec::wide<std::int32_t, eve::fixed<3>>;
	// ReSharper disable once CppInconsistentNaming
	using vec4i = vec::wide<std::int32_t, eve::fixed<4>>;

	// ReSharper disable once CppInconsistentNaming
	using vec2u = vec::wide<std::uint32_t, eve::fixed<2>>;
	// ReSharper disable once CppInconsistentNaming
	using vec3u = vec::wide<std::uint32_t, eve::fixed<3>>;
	// ReSharper disable once CppInconsistentNaming
	using vec4u = vec::wide<std::uint32_t, eve::fixed<4>>;
}
