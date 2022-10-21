#include <gsl/type/vector.hpp>

#include <span>
#include <gsl/utils/assert.hpp>
#include <gsl/core/core.hpp>

#include "gsl/utils/allocator.hpp"

namespace gal::gsl::type
{
	auto Vector::lock(core::ModuleContext& context) const -> void
	{
		if (shared || dead) { return; }

		++lock_;

		if (lock_ == 0) { context.throw_error("Vector lock overflow!"); }
	}

	auto Vector::unlock(core::ModuleContext& context) const -> void
	{
		if (shared || dead) { return; }

		if (lock_ == 0) { context.throw_error("Vector lock underflow!"); }

		--lock_;
	}

	auto Vector::reserve(core::ModuleContext& context, const size_type new_capacity, const type_descriptor::size_type type_size) -> void
	{
		if (is_locked()) { context.throw_error("Cannot change a vector's capacity if it locked"); }

		if (capacity_ >= new_capacity)
		{
			// nothing to do
			return;
		}

		const auto allocator = context.allocator()(__func__);

		if (auto* new_data = allocator->allocate(static_cast<utils::AllocatorBase::size_type>(new_capacity * type_size))) { context.throw_error("Cannot allocator more memory!"); }
		else
		{
			allocator->mark(new_data, {.comment = __func__});
			// copy data
			copy_into(vector_descriptor{}, new_data, data_, capacity_);
			data_ = new_data;
			capacity_ = new_capacity;
		}
	}

	auto Vector::resize(core::ModuleContext& context, const size_type new_size, const type_descriptor::size_type type_size, const bool zero_it) -> void
	{
		if (is_locked()) { context.throw_error("Cannot resize a vector if it locked"); }

		if (new_size > capacity_)
		{
			const auto new_capacity = std::ranges::max(
					size_type{16},
					size_type{1} << (std::countl_zero(size_type{}) - std::countl_zero(std::ranges::max(new_size, size_type{2}) - 1))
					);
			reserve(context, new_capacity, type_size);
		}

		if (zero_it && new_size > size_) { std::memset(data_ + size_ * type_size, 0, (new_size - size_) * type_size); }
		size_ = new_size;
	}

	auto Vector::clear(core::ModuleContext& context) -> void
	{
		if (is_locked()) { context.throw_error("Cannot clear a vector if it locked"); }

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

	auto vector_descriptor::copy_into_impl(void* dest, const void* source, const size_type count) -> void
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
