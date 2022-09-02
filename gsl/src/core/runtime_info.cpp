#include <gsl/core/runtime_info.hpp>

namespace gal::gsl::core
{
	void text_file_info::release_source_data() noexcept
	{
		if (owned_ && source_)
		{
			boost::gsl_allocator<value_type>{}.deallocate(const_cast<pointer>(source_), length_);
			source_ = nullptr;
			length_ = 0;
			owned_ = false;
		}
	}
}
