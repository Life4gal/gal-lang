#include <gsl/type/vector.hpp>

namespace gal::gsl::type
{
	auto Vector::lock(core::ModuleContext& context) const -> void
	{
		if (shared || dead) { return; }

		++lock_;

		// todo: context check lock overflow?
		(void)context;
	}

	auto Vector::unlock(core::ModuleContext& context) const -> void
	{
		if (shared || dead) { return; }

		// todo: context check lock underflow?
		(void)context;

		--lock_;
	}

	auto Vector::reserve(core::ModuleContext& context, const size_type new_capacity, const type_descriptor::size_type type_size) -> void
	{
		if (is_locked())
		{
			// todo: cannot change a vector's capacity if it locked
			return;
		}

		if (capacity_ >= new_capacity)
		{
			// nothing to do
			return;
		}

		// todo: do memory allocate
		(void)context;
		(void)type_size;
	}

	auto Vector::resize(core::ModuleContext& context, const size_type new_size, const type_descriptor::size_type type_size, const bool zero_it) -> void
	{
		if (is_locked())
		{
			(void)context;
			// todo: cannot change a vector's size if it locked
			return;
		}

		if (new_size > capacity_)
		{
			// todo: grow vector
		}

		if (zero_it && new_size > size_) { std::memset(data_ + size_ * type_size, 0, (new_size - size_) * type_size); }
		size_ = new_size;
	}

	auto Vector::clear(core::ModuleContext& context) -> void
	{
		if (is_locked())
		{
			(void)context;
			// todo: cannot clear a vector if it locked
			return;
		}

		size_ = 0;
	}

	auto Vector::clear(const data_type table_keys) -> bool
	{
		if (data_)
		{
			std::memset(data_, 0, table_keys - data_);
			size_ = 0;
			return true;
		}

		size_ = 0;
		return false;
	}
}
