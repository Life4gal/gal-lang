#pragma once

#include <gsl/accelerate/string_view.hpp>
#include <gsl/accelerate/vector.hpp>
#include <algorithm>

namespace gal::gsl::utils
{
	class StringPool
	{
	public:
		using value_type = accelerate::string_view;

		using char_type = value_type::value_type;
		using size_type = value_type::size_type;
		using trait_type = value_type::traits_type;

	private:
		// note: not null terminate
		class StringBlock
		{
		public:
			using pointer = value_type::pointer;

		private:
			size_type capacity_;
			size_type size_;
			pointer data_;

		public:
			explicit StringBlock(size_type capacity);

			StringBlock(const StringBlock& other) = delete;
			auto operator=(const StringBlock& other) -> StringBlock& = delete;

			StringBlock(StringBlock&& other) noexcept
				: capacity_{std::exchange(other.capacity_, 0)},
				size_{std::exchange(other.size_, 0)},
				data_{std::exchange(other.data_, nullptr)} {}

			auto operator=(StringBlock&& other) noexcept -> StringBlock&
			{
				capacity_ = std::exchange(other.capacity_, 0);
				size_ = std::exchange(other.size_, 0);
				data_ = std::exchange(other.data_, nullptr);

				return *this;
			}

			~StringBlock() noexcept;

			[[nodiscard]] auto append(value_type string) -> value_type;

			[[nodiscard]] constexpr auto size() const noexcept -> size_type { return size_; }

			[[nodiscard]] constexpr auto capacity() const noexcept -> size_type { return capacity_; }

			[[nodiscard]] constexpr auto available() const noexcept -> size_type { return capacity() - size(); }

			[[nodiscard]] constexpr auto storable(const value_type string) const noexcept -> bool { return available() >= string.length(); }

			[[nodiscard]] constexpr auto storable(const size_type size) const noexcept -> bool { return available() >= size; }

			[[nodiscard]] constexpr auto more_available_than(const StringBlock& other) const noexcept -> bool { return available() > other.available(); }
		};

		using block_type = StringBlock;
		using pool_type = accelerate::vector<block_type>;
		using iterator = pool_type::iterator;

	public:
		constexpr static size_type default_capacity = 1 << 16;

	private:
		pool_type pool_;
		size_type current_capacity_;

		[[nodiscard]] auto append_str_into_block(const value_type string, iterator pos) -> value_type
		{
			const auto ret = pos->append(string);

			shake_it(pos);
			return ret;
		}

		[[nodiscard]] constexpr auto find_or_create_block(const size_type size) -> iterator
		{
			if (const auto pos = find_storable_block(size);
				pos != pool_.end()) { return pos; }
			return create_storable_block(size);
		}

		[[nodiscard]] constexpr auto find_or_create_block(const value_type string) -> iterator { return find_or_create_block(string.length()); }

		[[nodiscard]] constexpr auto find_first_possible_storable_block(const size_type size) noexcept -> iterator
		{
			if (pool_.size() > 2 && not std::ranges::prev(pool_.end(), 2)->storable(size)) { return std::ranges::prev(pool_.end()); }
			return pool_.begin();
		}

		[[nodiscard]] constexpr auto find_first_possible_storable_block(const value_type string) noexcept -> iterator { return find_first_possible_storable_block(string.length()); }

		[[nodiscard]] constexpr auto find_storable_block(const size_type size) noexcept -> iterator
		{
			return std::ranges::lower_bound(
					find_first_possible_storable_block(size),
					pool_.end(),
					true,
					[](const bool b, bool) { return b; },
					[size](const auto& block) { return not block.storable(size); });
		}

		[[nodiscard]] constexpr auto find_storable_block(const value_type string) noexcept -> iterator { return find_storable_block(string.size()); }

		[[nodiscard]] constexpr auto create_storable_block(const size_type size) -> iterator
		{
			pool_.emplace_back(std::ranges::max(current_capacity_, size));
			return std::ranges::prev(pool_.end());
		}

		[[nodiscard]] constexpr auto create_storable_block(const value_type string) -> iterator { return create_storable_block(string.size()); }

		constexpr auto shake_it(iterator block) -> void
		{
			if (
				block == pool_.begin() ||
				block->more_available_than(*std::ranges::prev(block))) { return; }

			if (const auto it =
						std::ranges::upper_bound(
								pool_.begin(),
								block,
								block->available(),
								std::ranges::less{},
								[](const auto& b) { return b.available(); });
				it != block) { std::ranges::rotate(it, block, std::ranges::next(block)); }
		}

	public:
		constexpr explicit StringPool(const size_type capacity = default_capacity) noexcept(std::is_nothrow_default_constructible_v<pool_type>)
			: current_capacity_{capacity} {}

		template<std::same_as<StringPool>... Pools>
		constexpr auto takeover(Pools&&... pools) -> void
		{
			pool_.reserve(pool_.size() + (pools.pool_.size() + ...));

			iterator it;
			(((it = pool_.insert(pool_.end(), std::make_move_iterator(pools.pool_.begin()), std::make_move_iterator(pools.pool_.end()))),
			pools.pool_.clear(),
			std::ranges::inplace_merge(pool_.begin(), it, pool_.end(), [](const auto& a, const auto& b) { return not a.more_available_than(b); })),
				...);
		}

		auto append(const value_type string) -> value_type { return append_str_into_block(string, find_or_create_block(string)); }

		[[nodiscard]] constexpr auto size() const noexcept -> size_type { return pool_.size(); }

		[[nodiscard]] constexpr auto capacity() const noexcept -> size_type { return current_capacity_; }

		constexpr auto set_capacity(const size_type new_capacity) noexcept -> void { current_capacity_ = new_capacity; }

		constexpr auto clear() noexcept -> void { pool_.clear(); }
	};
}
