#pragma once

#include <type_traits>

namespace gal::gsl::core
{
	enum class is_reference
	{
		yes,
		no,
	};

	enum class is_const
	{
		yes,
		no,
	};

	// ReSharper disable once IdentifierTypo
	enum class is_xvalue
	{
		yes,
		no,
	};

	template<typename T>
	struct is_cloneable : std::false_type { };

	template<typename T>
		requires(std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>)
	struct is_cloneable<T> : std::true_type { };

	template<typename T>
	constexpr static bool is_cloneable_v = is_cloneable<T>::value;
}
