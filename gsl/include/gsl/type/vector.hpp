#pragma once

#include <gsl/core/value.hpp>
#include <gsl/type/type_descriptor.hpp>

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
			auto clear(data_type table_keys) -> bool;

			[[nodiscard]] auto values() const -> std::add_const_t<data_type> { return data_; }
		};

		struct vector_descriptor : public type_descriptor
		{
			friend type_descriptor;

		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(Vector); }

			static auto zero_out_impl(void* dest, size_type count) noexcept -> void;

			static auto copy_into_impl(void* dest, const void* source, size_type count) -> void;

			static auto move_into_impl(void* dest, void* source, size_type count) -> void;
		};
	}

	namespace core
	{
		// todo
		template<>
		class ValueCaster<type::Vector> : public value_caster_undefined
		{
		public:
			constexpr static auto from(const type::Vector& data) noexcept -> Value = delete;

			constexpr static auto to(const Value& data) noexcept -> type::Vector = delete;
		};
	}
}
