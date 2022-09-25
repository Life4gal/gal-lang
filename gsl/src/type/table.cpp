#include <gsl/type/table.hpp>
#include <gsl/utils/hash.hpp>

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


	auto Table::lock(core::ModuleContext& context) const -> void { data_.lock(context); }

	auto Table::unlock(core::ModuleContext& context) const -> void { data_.unlock(context); }

	auto Table::clear(core::ModuleContext& context) -> void
	{
		if (is_locked())
		{
			(void)context;
			// todo: cannot clear a table if it locked
			return;
		}

		if (data_.clear(keys_)) { std::memset(hashes_, 0, data_.capacity() * sizeof(hash_type)); }
	}

	auto Table::keys(core::ModuleContext& context, const size_type type_size) -> sequence
	{
		// todo
		(void)context;
		(void)type_size;
		return {};
	}

	auto Table::values(core::ModuleContext& context, const size_type type_size) -> sequence
	{
		// todo
		(void)context;
		(void)type_size;
		return {};
	}
}
