#pragma once

#include <type_traits>

namespace gal::gsl::type
{
	struct type_descriptor
	{
		using size_type = std::size_t;

		[[nodiscard]] constexpr auto type_size(this const auto& self) noexcept -> size_type { return self.type_size_impl(); }

		// todo: more information
	};

	template<typename TypeDescriptor>
		requires std::is_convertible_v<TypeDescriptor, type_descriptor>
	[[nodiscard]] constexpr auto type_size(const TypeDescriptor& descriptor) noexcept -> type_descriptor::size_type { return descriptor.type_size(); }
}
