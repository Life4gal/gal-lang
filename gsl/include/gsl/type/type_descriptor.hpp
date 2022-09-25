#pragma once

#include <type_traits>
#include <algorithm>

namespace gal::gsl::type
{
	struct type_descriptor
	{
		using size_type = std::size_t;
		using raw_memory_type = char*;

		[[nodiscard]] constexpr auto type_size(this const auto& self) noexcept -> size_type { return self.type_size_impl(); }

		constexpr auto zero_out(this const auto& self, void* dest, const size_type count) -> void { return self.zero_out_impl(dest, count); }

		constexpr auto copy_into(this const auto& self, void* dest, const void* source, const size_type count) -> void { self.copy_into_impl(dest, source, count); }

		constexpr auto move_into(this const auto& self, void* dest, void* source, const size_type count) -> void { self.move_into_impl(dest, source, count); }

		// todo: more information
	};

	template<typename TypeDescriptor>
		requires std::is_convertible_v<TypeDescriptor, type_descriptor>
	[[nodiscard]] constexpr auto type_size(const TypeDescriptor& descriptor) noexcept -> type_descriptor::size_type { return descriptor.type_size(); }

	template<typename TypeDescriptor>
		requires std::is_convertible_v<TypeDescriptor, type_descriptor>
	constexpr auto zero_out(const TypeDescriptor& descriptor, void* dest, const type_descriptor::size_type count) -> void { return descriptor.zero_out(dest, count); }

	template<typename TypeDescriptor>
		requires std::is_convertible_v<TypeDescriptor, type_descriptor>
	constexpr auto copy_info(const TypeDescriptor& descriptor, void* dest, const void* source, const type_descriptor::size_type count) -> void { descriptor.copy_info(dest, source, count); }

	template<typename TypeDescriptor>
		requires std::is_convertible_v<TypeDescriptor, type_descriptor>
	constexpr auto move_into(const TypeDescriptor& descriptor, void* dest, void* source, const type_descriptor::size_type count) -> void { descriptor.move_info(dest, source, count); }
}
