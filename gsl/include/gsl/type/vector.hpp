#pragma once

#include <span>
#include <gsl/core/value.hpp>
#include <gsl/type/type_descriptor.hpp>
#include <gsl/utils/assert.hpp>

namespace gal::gsl
{
	namespace core
	{
		class ModuleContext;
	}

	namespace type
	{
		class Vector
		{
			friend struct vector_descriptor;
		public:
			using data_type = type_descriptor::raw_memory_type;
			using size_type = std::uint32_t;
			using lock_type = std::uint32_t;
			using flag_type = std::uint32_t;

		private:
			data_type data_{nullptr};
			size_type size_{0};
			size_type capacity_{0};
			mutable lock_type lock_{0};

			union
			{
				flag_type flag;

				flag_type shared : 1;
				flag_type dead : 1;
				// more flags?
			};

		public:
			[[nodiscard]] constexpr auto size() const noexcept -> size_type { return size_; }
			[[nodiscard]] constexpr auto capacity() const noexcept -> size_type { return capacity_; }
			[[nodiscard]] constexpr auto is_locked() const noexcept -> lock_type { return lock_; }
			[[nodiscard]] constexpr auto lock_count() const noexcept -> lock_type { return lock_; }

			auto lock(core::ModuleContext& context) const -> void;
			auto unlock(core::ModuleContext& context) const -> void;

			auto reserve(core::ModuleContext& context, size_type new_capacity, type_descriptor::size_type type_size) -> void;
			auto resize(core::ModuleContext& context, size_type new_size, type_descriptor::size_type type_size, bool zero_it) -> void;
			auto clear(core::ModuleContext& context) -> void;

		private:
			// for table
			friend class Table;

			// This `clear` does not check the `lock`!
			auto clear(const data_type table_keys) -> bool;
		};

		struct vector_descriptor : public type_descriptor
		{
			friend type_descriptor;

		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(Vector); }

			constexpr static auto zero_out_impl(void* dest, const size_type count) noexcept -> void
			{
				std::ranges::for_each(
						static_cast<Vector*>(dest),
						static_cast<Vector*>(dest) + count,
						[](Vector& d) { d = {}; });
			}

			constexpr static auto copy_into_impl(void* dest, const void* source, const size_type count) -> void
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

			constexpr static auto move_into_impl(void* dest, void* source, const size_type count) -> void
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
		};
	}

	namespace core
	{
		// todo
		template<>
		struct ValueCaster<type::Vector>
		{
			constexpr static auto from(const type::Vector data) noexcept -> Value = delete;

			constexpr static auto to(const Value& data) noexcept -> type::Vector = delete;
		};
	}
}
