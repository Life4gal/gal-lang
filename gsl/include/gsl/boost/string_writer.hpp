#pragma once

#include <string_view>
#include <vector>
#include <gsl/core/macro.hpp>
#include <gsl/boost/logger.hpp>

namespace gal::gsl::boost
{
	namespace string_writer_detail
	{
		constexpr std::size_t fixed_buffer_size = 1 << 12;

		class fixed_buffer
		{
		public:
			using value_type = char;

			using buffer_type = value_type[fixed_buffer_size];
			using size_type = std::size_t;

			using pointer = value_type*;
			using const_pointer = const value_type*;

			using iterator = pointer;
			using const_iterator = const_pointer;

			using view_type = std::basic_string_view<value_type>;

		private:
			alignas(alignof(std::max_align_t)) buffer_type data_{};
			size_type size_{};

		public:
			constexpr pointer allocate(const size_type size)
			{
				if (size + size_ <= fixed_buffer_size)
				{
					const pointer current = data_ + size_;
					size_ += size;
					return current;
				}

				logger::fatal(
						"{} --> buffer overflow, a additional size {} is required, but the remaining size is {}. (Maximum size is {}.)",
						__func__,
						size,
						fixed_buffer_size - size_,
						fixed_buffer_size);
				GSL_UNREACHABLE();
			}

			constexpr void append(const const_pointer source, const size_type size)
			{
				if (size + size_ <= fixed_buffer_size)
				{
					std::memcpy(data_ + size_, source, size);
					size_ += size;
					return;
				}

				logger::fatal(
						"{} --> buffer overflow, a additional size {} is required, but the remaining size is {}. (Maximum size is {}.)",
						__func__,
						size,
						fixed_buffer_size - size_,
						fixed_buffer_size);
				GSL_UNREACHABLE();
			}

			constexpr void append(const view_type view) { append(view.data(), view.size()); }

			constexpr void push_back(const value_type value)
			{
				if (1 + size_ <= fixed_buffer_size)
				{
					data_[size_] = value;
					size_ += 1;
					return;
				}

				logger::fatal(
						"{} --> buffer overflow, a additional size {} is required, but the remaining size is {}. (Maximum size is {}.)",
						__func__,
						1,
						fixed_buffer_size - size_,
						fixed_buffer_size);
				GSL_UNREACHABLE();
			}

			[[nodiscard]] constexpr size_type size() const noexcept { return size_; }

			[[nodiscard]] constexpr view_type view() const noexcept { return {data_, size_}; }
		};

		class vector_buffer
		{
		public:
			using value_type = fixed_buffer::value_type;

			using buffer_type = std::vector<value_type>;
			using size_type = buffer_type::size_type;

			using pointer = buffer_type::pointer;
			using const_pointer = buffer_type::const_pointer;

			using iterator = buffer_type::iterator;
			using const_iterator = buffer_type::const_iterator;

			using view_type = std::basic_string_view<value_type>;

		private:
			buffer_type data_{};

		public:
			constexpr pointer allocate(const size_type size)
			{
				const pointer current = data_.data();
				data_.resize(data_.size() + size);
				return current;
			}

			constexpr void append(const const_pointer source, const size_type size)
			{
				data_.reserve(data_.size() + size);
				data_.insert(data_.end(), source, source + size);
			}

			constexpr void append(const view_type view) { append(view.data(), view.size()); }

			constexpr void push_back(const value_type value) { data_.push_back(value); }

			[[nodiscard]] constexpr size_type size() const noexcept { return data_.size(); }

			[[nodiscard]] constexpr view_type view() const noexcept { return {data_.data(), data_.size()}; }
		};

		template<typename StringWriter>
		class string_writer_back_insert_iterator
		{
		public:
			using container_type = StringWriter;

			using iterator = typename container_type::buffer_type::iterator;

			// using iterator_concept = typename std::iterator_traits<iterator>::iterator_concept;
			using iterator_category = typename std::iterator_traits<iterator>::iterator_category;
			using value_type = typename std::iterator_traits<iterator>::value_type;
			using difference_type = typename std::iterator_traits<iterator>::difference_type;
			using pointer = typename std::iterator_traits<iterator>::pointer;
			using reference = typename std::iterator_traits<iterator>::reference;

		private:
			container_type* container_;

		public:
			constexpr explicit string_writer_back_insert_iterator(container_type& container)
				: container_{std::addressof(container)} {}

			constexpr string_writer_back_insert_iterator& operator=(const value_type& value)
			{
				container_->push_back(value);
				return *this;
			}

			[[nodiscard]] constexpr string_writer_back_insert_iterator& operator*() { return *this; }

			[[nodiscard]] constexpr string_writer_back_insert_iterator& operator++() { return *this; }

			[[nodiscard]] constexpr string_writer_back_insert_iterator& operator++(int) { return *this; }
		};

		template<typename BufferType>
			requires(std::is_same_v<BufferType, fixed_buffer> or std::is_same_v<BufferType, vector_buffer>)
		class string_writer
		{
		public:
			using buffer_type = BufferType;

			using value_type = typename buffer_type::value_type;
			using size_type = typename buffer_type::size_type;

			using pointer = typename buffer_type::pointer;
			using const_pointer = typename buffer_type::const_pointer;

			using view_type = typename buffer_type::view_type;

		private:
			buffer_type buffer_;

		public:
			constexpr string_writer& push_back(const value_type value)
			{
				buffer_.push_back(value);
				return *this;
			}

			constexpr string_writer& append(const const_pointer source, const size_type size)
			{
				buffer_.append(source, size);
				return *this;
			}

			constexpr string_writer& append(const view_type view)
			{
				buffer_.append(view);
				return *this;
			}

			[[nodiscard]] constexpr size_type size() const noexcept { return buffer_.size(); }

			[[nodiscard]] constexpr view_type view() const noexcept { return buffer_.view(); }
		};
	}

	using fixed_string_writer = string_writer_detail::string_writer<string_writer_detail::fixed_buffer>;
	using text_string_writer = string_writer_detail::string_writer<string_writer_detail::vector_buffer>;

	template<typename StringWrite>
	[[nodiscard]] constexpr string_writer_detail::string_writer_back_insert_iterator<StringWrite> back_insert(StringWrite& sw) { return string_writer_detail::string_writer_back_insert_iterator<StringWrite>{sw}; }
}
