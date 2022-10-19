#include <gsl/utils/string_pool.hpp>

#include <gsl/config.hpp>
#include <gsl/utils/assert.hpp>
#include <fmt/format.h>

namespace gal::gsl::utils
{
	StringPool::StringBlock::StringBlock(const size_type capacity)
		: capacity_{capacity},
		size_{0},
		data_{static_cast<pointer>(heap::allocate(static_cast<heap::size_type>(capacity)))} {}

	StringPool::StringBlock::~StringBlock() noexcept { heap::deallocate(data_); }

	auto StringPool::StringBlock::append(value_type string) -> value_type
	{
		if (!storable(string))
		{
			gsl_trap(fmt::format("There are not enough space for string `{}`.", string));
			return {};
		}

		const auto dest = data_ + size_;
		std::ranges::copy(string, dest);

		size_ += string.length();
		return value_type{dest, string.length()};
	}
}
