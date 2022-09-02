#pragma once

#include <gsl/core/core_detail/runtime_info.hpp>

namespace gal::gsl::core
{
	class type_info_struct;
	class type_info_enum;

	class type_annotation;

	class type_info
	{
		enum class flags
		{
			is_pod = 1 << 0,
			is_raw_pos = 1 << 1,
			is_const = 1 << 2,
			// ReSharper disable once IdentifierTypo
			is_xvalue = 1 << 3,
			is_ref = 1 << 4,
			is_ref_type = 1 << 5,
			is_ref_value = 1 << 6,
			is_default_init_value = 1 << 7,
			is_smart_pointer = 1 << 8,
			is_smart_pointer_native = 1 << 9,
			// todo
		};

	public:
		using unchecked_type = union
		{
			type_info_struct* type_struct;
			type_info_enum* type_enum;
			type_annotation* type_annotation;
		};

		enum class unchecked_type_index
		{
			is_struct_info,
			is_enum_info,
			is_type_annotation_info,
		};

	private:
		runtime_type type_;
		unchecked_type holding_;
		flags flag_;
		// todo

	public:
		template<typename... Flag>
			requires(std::is_same_v<Flag, flags> && ...)
		[[nodiscard]] constexpr bool is(const Flag ... fs) const noexcept { return ((static_cast<std::underlying_type_t<flags>>(flag_) & static_cast<std::underlying_type_t<flags>>(fs)) && ...); }

		[[nodiscard]] type_annotation* get_annotation() const;
	};
}
