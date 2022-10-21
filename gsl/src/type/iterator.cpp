#include <gsl/type/iterator.hpp>

#include <gsl/core/core.hpp>
#include <gsl/utils/allocator.hpp>

namespace gal::gsl::type
{
	Iterator::~Iterator() = default;

	auto TrivialIterator::begin(core::ModuleContext& context, const data_type output) -> bool
	{
		(void)context;
		if (begin_ != end_)
		{
			*reinterpret_cast<pointer>(output) = *begin_;
			return true;
		}
		return false;
	}

	auto TrivialIterator::next(core::ModuleContext& context, const data_type output) -> bool
	{
		(void)context;
		if (++begin_ != end_)
		{
			*reinterpret_cast<pointer>(output) = *begin_;
			return true;
		}
		return false;
	}

	auto TrivialIterator::done(core::ModuleContext& context) -> void
	{
		const auto allocator = context.allocator()(__func__);

		allocator->deallocate(reinterpret_cast<utils::AllocatorBase::data_type>(this), static_cast<utils::AllocatorBase::size_type>(size_));
	}
}
