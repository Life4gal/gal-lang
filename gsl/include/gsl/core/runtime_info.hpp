#pragma once

#define GSL_ENABLE_PROFILER

#include <gsl/core/core_detail/type_traits.hpp>
#include <gsl/core/core_detail/line_info.hpp>
#include <gsl/core/core_detail/file_info.hpp>
#include <gsl/core/core_detail/file_accessor.hpp>
#include <gsl/core/core_detail/core_type.hpp>

namespace gal::gsl::core
{
	class text_file_info final : public file_info
	{
	public:
		using value_type = accelerate::string::value_type;
		using size_type = accelerate::string::size_type;

		using pointer = accelerate::string::pointer;
		using const_pointer = accelerate::string::const_pointer;

	private:
		const_pointer source_;

		size_type length_ : (sizeof(size_type) * 8 - 1);
		size_type owned_ : 1;

	public:
		constexpr text_file_info(const const_pointer source, const size_type length, const bool owned)
			: source_{source},
			  length_{length},
			  owned_{owned} {}

		constexpr text_file_info(const source_view_type source, const bool owned)
			: source_{source.data()},
			  length_{source.size()},
			  owned_{owned} {}

		void release_source_data() noexcept override;

		[[nodiscard]] constexpr source_view_type get_source_view() const noexcept override { return {source_, length_}; }
	};

	class module_context;
	class module_function;

	class module_file_accessor final : public file_accessor
	{
	protected:
		// accelerate::unique_ptr<module_context> context_;
		module_context* context_{nullptr};
		module_function* module_getter_{nullptr};
		module_function* module_include_getter_{nullptr};
		module_function* module_allow_checker_{nullptr};

	public:
		module_file_accessor() = default;
		module_file_accessor(file_name_view_type pak, const file_accessor_ptr& accessor);

		[[nodiscard]] bool valid() const noexcept { return context_ && module_getter_; }

		[[nodiscard]] file_name_type splice_include_path(file_name_view_type filename, file_name_view_type include_filename) const override;
		[[nodiscard]] module_info_view get_module_info(file_name_view_type request, file_name_view_type from) const override;
		[[nodiscard]] bool is_module_allowed(file_name_view_type module_name, file_name_view_type filename) const override;
	};

	template<>
	struct is_cloneable<module_file_accessor> : std::false_type {};
}
