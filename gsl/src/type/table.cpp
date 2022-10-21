#include <gsl/type/table.hpp>
#include <gsl/utils/hash.hpp>
#include <gsl/utils/allocator.hpp>
#include <gsl/core/core.hpp>
#include <gsl/utils/assert.hpp>

namespace gal::gsl::type
{
	auto TableIterator::next_valid_index(std::ptrdiff_t index) const noexcept -> hash_type
	{
		for (; index < table_.capacity(); ++index) { if (table_.hash(index) > utils::kGslHash64Killed) { break; } }
		return index;
	}


	auto TableIterator::begin(core::ModuleContext& context, const data_type output) -> bool
	{
		table_.lock(context);

		begin_ = data();
		end_ = begin_ + static_cast<std::ptrdiff_t>(table_.capacity() * type_size_);

		begin_ += next_valid_index(0) * type_size_;
		*reinterpret_cast<pointer>(output) = begin_;
		return table_.size();
	}

	auto TableIterator::next(core::ModuleContext& context, const data_type output) -> bool
	{
		(void)context;
		const auto data = this->data();
		const auto index = next_valid_index((begin_ - data) / type_size_ + 1);
		begin_ = data + index * type_size_;
		*reinterpret_cast<pointer>(output) = begin_;
		return begin_ != end_;
	}

	auto TableIterator::done(core::ModuleContext& context) -> void { table_.unlock(context); }

	auto TableKeysIterator::done(core::ModuleContext& context) -> void
	{
		TableIterator::done(context);
		// todo: free self
	}

	auto TableKeysIterator::data() const -> data_type { return table_.keys(); }

	auto TableValuesIterator::done(core::ModuleContext& context) -> void
	{
		TableIterator::done(context);
		// todo self
	}

	auto TableValuesIterator::data() const -> data_type { return table_.values(); }

	auto Table::clear(core::ModuleContext& context) -> void
	{
		if (is_locked()) { context.throw_error("Cannot clear a table if it locked"); }

		if (data_.clear(keys_)) { std::memset(hashes_, 0, data_.capacity() * sizeof(hash_type)); }
	}

	auto Table::keys(core::ModuleContext& context, const size_type type_size) -> sequence
	{
		const auto allocator = context.allocator()(__func__);
		// allocator iterator
		auto* iterator = allocator->allocate(sizeof(TableKeysIterator));
		allocator->mark(iterator, utils::FreeGrowModel::big_stuff_info{.comment = "table key iterator"});

		std::construct_at(reinterpret_cast<TableKeysIterator*>(iterator), *this, type_size);
		return {.iterator = reinterpret_cast<TableKeysIterator*>(iterator)};
	}

	auto Table::values(core::ModuleContext& context, const size_type type_size) -> sequence
	{
		const auto allocator = context.allocator()(__func__);
		// allocator iterator
		auto* iterator = allocator->allocate(sizeof(TableValuesIterator));
		allocator->mark(iterator, utils::FreeGrowModel::big_stuff_info{.comment = "table value iterator"});

		std::construct_at(reinterpret_cast<TableValuesIterator*>(iterator), *this, type_size);
		return {.iterator = reinterpret_cast<TableValuesIterator*>(iterator)};
	}

	auto Table::hash(const std::ptrdiff_t index) const noexcept -> hash_type
	{
		gsl_assert(index < capacity() && index >= 0, "Index out of range!");
		return hashes_[index];
	}
}
