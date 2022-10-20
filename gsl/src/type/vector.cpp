#include <gsl/type/vector.hpp>

#include <span>
#include <gsl/utils/assert.hpp>

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

	auto vector_descriptor::zero_out_impl(void* dest, const size_type count) noexcept -> void
	{
		std::ranges::for_each(
				static_cast<Vector*>(dest),
				static_cast<Vector*>(dest) + count,
				[](Vector& d) { d = {}; });
	}

	auto vector_descriptor::copy_into_impl(void* dest, const void* source, size_type count) -> void
	{
		const auto vector_source = std::span{static_cast<const Vector*>(source), count};
		std::ranges::for_each(
				vector_source,
				[dest = static_cast<Vector*>(dest)](const auto& s) mutable
				{
					dest->size_ = s.size_;
					dest->capacity_ = s.capacity_;
					// todo: how about lock?
					// dest->lock_ = s.lock_;
					// todo: how about non-trivial type?
					gsl_assert(dest->data_ != nullptr, "dest vector should allocate memory before copy!");
					std::ranges::copy(s.data_, s.data_ + s.size_, dest->data_);
					// next one
					++dest;
				});
	}

	auto vector_descriptor::move_into_impl(void* dest, void* source, size_type count) -> void
	{
		auto vector_source = std::span{static_cast<Vector*>(source), count};
		std::ranges::for_each(
				vector_source,
				[dest = static_cast<Vector*>(dest)](auto& s) mutable
				{
					dest->size_ = std::exchange(s.size_, 0);
					dest->capacity_ = std::exchange(s.capacity_, 0);
					// todo: how about lock?
					// dest->lock_		= std::exchange(s.lock_, 0);
					dest->data_ = std::exchange(s.data_, nullptr);
					// next one
					++dest;
				});
	}
}
