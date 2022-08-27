#pragma once

#include <cstddef>
#include <gsl/accelerate/string.hpp>
#include <gsl/accelerate/string_view.hpp>

#ifndef GAL_SCRIPT_LANG_FIXED_BUFFER_SIZE
#define GAL_SCRIPT_LANG_FIXED_BUFFER_SIZE 1 << 12
#endif

namespace gal::gsl::utils
{
	namespace string_writer_detail
	{
		class fixed_buffer
		{
			using string_type = accelerate::string;
			using string_view_type = accelerate::string_view;
		public:
			using value_type = string_type::value_type;
			using size_type = string_type::size_type;

			constexpr static size_type fixed_buffer_size = GAL_SCRIPT_LANG_FIXED_BUFFER_SIZE;

			using buffer_type = value_type[fixed_buffer_size];

			using pointer = string_type::pointer;
			using const_pointer = string_type::const_pointer;

			using iterator = pointer;
			using const_iterator = const_pointer;

			using view_type = string_view_type;

		private:
			alignas(alignof(std::max_align_t)) buffer_type data_{};
			size_type size_{};

		public:
			pointer allocate(size_type size);

			void append(const_pointer source, size_type size);

			void append(const view_type view) { append(view.data(), view.size()); }

			void push_back(value_type value);

			[[nodiscard]] constexpr size_type size() const noexcept { return size_; }

			[[nodiscard]] constexpr view_type view() const noexcept { return {data_, size_}; }
		};

		class vector_buffer
		{
			using string_type = accelerate::string;
			using string_view_type = accelerate::string_view;
		public:
			using value_type = string_type::value_type;
			using size_type = string_type::size_type;

			using pointer = string_type::pointer;
			using const_pointer = string_type::const_pointer;

			using iterator = string_type::iterator;
			using const_iterator = string_type::const_iterator;

			using view_type = string_view_type;
			using source_type = string_type;

		private:
			string_type data_{};

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

			// ReSharper disable once CppNonExplicitConversionOperator
			[[nodiscard]] constexpr operator source_type() && noexcept { return std::move(data_); }
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

		template<typename T>
		struct delay_source_type
		{
			using type = void;
		};

		template<>
		struct delay_source_type<vector_buffer>
		{
			using type = vector_buffer::source_type;
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

			// ReSharper disable once CppNonExplicitConversionOperator
			[[nodiscard]] constexpr operator typename delay_source_type<buffer_type>::type() && noexcept
				requires std::is_same_v<buffer_type, vector_buffer> { return std::move(buffer_).operator typename buffer_type::source_type(); }
		};
	}

	using fixed_string_writer = string_writer_detail::string_writer<string_writer_detail::fixed_buffer>;
	using text_string_writer = string_writer_detail::string_writer<string_writer_detail::vector_buffer>;

	template<typename StringWrite>
	[[nodiscard]] constexpr string_writer_detail::string_writer_back_insert_iterator<StringWrite> back_insert(StringWrite& sw) { return string_writer_detail::string_writer_back_insert_iterator<StringWrite>{sw}; }
}
