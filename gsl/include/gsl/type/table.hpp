#pragma once

#include <gsl/type/iterator.hpp>
#include <gsl/type/vector.hpp>

namespace gal::gsl::type
{
	class Table;

	class TableIterator : public Iterator
	{
	public:
		using data_type = Vector::data_type;
		using size_type = Vector::size_type;
		using hash_type = std::uint64_t;
		using lock_type = Vector::lock_type;

	private:
		const Table& table_;
		size_type type_size_;
		data_type begin_;
		data_type end_;

	public:
		TableIterator(const Table& table, const size_type type_size)
			: table_{table},
			type_size_{type_size},
			begin_{nullptr},
			end_{nullptr} {}

		auto begin(core::ModuleContext& context, data_type output) -> bool override;
		auto next(core::ModuleContext& context, data_type output) -> bool override;

		auto done(core::ModuleContext& context) -> void override;

		[[nodiscard]] auto next_valid_index(std::ptrdiff_t index) const noexcept -> hash_type;

		[[nodiscard]] virtual auto data() const -> data_type = 0;
	};

	class Table
	{
	public:
		using data_type = TableIterator::data_type;
		using hash_type = TableIterator::hash_type;
		using size_type = TableIterator::size_type;
		using lock_type = TableIterator::lock_type;

	private:
		data_type keys_{nullptr};
		hash_type* hashes_{nullptr};
		size_type max_lookups_{0};
		size_type shift_{0};
		Vector data_{};

	public:
		[[nodiscard]] constexpr auto size() const noexcept -> size_type { return data_.size(); }
		[[nodiscard]] constexpr auto capacity() const noexcept -> size_type { return data_.capacity(); }
		[[nodiscard]] constexpr auto is_locked() const noexcept -> lock_type { return data_.is_locked(); }
		[[nodiscard]] constexpr auto lock_count() const noexcept -> lock_type { return data_.lock_count(); }

		auto lock(core::ModuleContext& context) const -> void;
		auto unlock(core::ModuleContext& context) const -> void;

		auto clear(core::ModuleContext& context) -> void;

		[[nodiscard]] auto keys(core::ModuleContext& context, size_type type_size) -> sequence;
		[[nodiscard]] auto values(core::ModuleContext& context, size_type type_size) -> sequence;

		[[nodiscard]] auto			 hash(const std::ptrdiff_t index) const noexcept -> hash_type
		{
			gsl_assert(index < capacity(), "index out of range!");
			return hashes_[index];
		}
	};
}
