#include <gsl/utils/string_writer.hpp>

#include <gsl/core/macro.hpp>
#include <gsl/boost/logger.hpp>

namespace gal::gsl::utils::string_writer_detail
{
	fixed_buffer::pointer fixed_buffer::allocate(size_type size)
	{
		if (size + size_ <= fixed_buffer_size)
		{
			const pointer current = data_ + size_;
			size_ += size;
			return current;
		}

		boost::logger::fatal(
				"{} --> buffer overflow, a additional size {} is required, but the remaining size is {}. (Maximum size is {}.)",
				__func__,
				size,
				fixed_buffer_size - size_,
				fixed_buffer_size);
		GSL_UNREACHABLE();
	}

	void fixed_buffer::append(const const_pointer source, const size_type size)
	{
		if (size + size_ <= fixed_buffer_size)
		{
			std::memcpy(data_ + size_, source, size);
			size_ += size;
			return;
		}

		boost::logger::fatal(
				"{} --> buffer overflow, a additional size {} is required, but the remaining size is {}. (Maximum size is {}.)",
				__func__,
				size,
				fixed_buffer_size - size_,
				fixed_buffer_size);
		GSL_UNREACHABLE();
	}

	void fixed_buffer::push_back(const value_type value)
	{
		if (1 + size_ <= fixed_buffer_size)
		{
			data_[size_] = value;
			size_ += 1;
			return;
		}

		boost::logger::fatal(
				"{} --> buffer overflow, a additional size {} is required, but the remaining size is {}. (Maximum size is {}.)",
				__func__,
				1,
				fixed_buffer_size - size_,
				fixed_buffer_size);
		GSL_UNREACHABLE();
	}
}
