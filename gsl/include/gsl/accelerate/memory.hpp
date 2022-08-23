#pragma once

#include <memory>

namespace gal::gsl::accelerate
{
	template<typename T, typename Deleter = ::std::default_delete<T>>
	using unique_ptr = ::std::unique_ptr<T, Deleter>;

	template<typename T>
	using shared_ptr = ::std::shared_ptr<T>;

	template<typename T>
	using enable_shared_from_this = ::std::enable_shared_from_this<T>;
}
