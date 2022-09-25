#include <gsl/type/iterator.hpp>

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
		// todo: deallocate this
		(void)context;
	}
}
