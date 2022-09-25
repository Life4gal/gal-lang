#pragma once

#include <gsl/type/type_descriptor.hpp>

namespace gal::gsl
{
	namespace core
	{
		class ModuleContext;
	}

	namespace type
	{
		class Iterator
		{
		public:
			using data_type = type_descriptor::raw_memory_type;
			using size_type = std::uint32_t;

			using pointer = data_type*;

			virtual ~Iterator();

			virtual auto begin(core::ModuleContext& context, data_type output) -> bool = 0;
			virtual auto next(core::ModuleContext& context, data_type output) -> bool = 0;

			virtual auto done(core::ModuleContext& context) -> void = 0;
		};

		struct sequence final
		{
			// heap allocate!
			Iterator* iterator;
		};

		class TrivialIterator final : public Iterator
		{
		public:
		private:
			pointer begin_;
			pointer end_;
			size_type size_;

		public:
			TrivialIterator(const pointer begin, const pointer end, const size_type size)
				: begin_{begin},
				end_{end},
				size_{size} {}

			auto begin(core::ModuleContext& context, data_type output) -> bool override;
			auto next(core::ModuleContext& context, data_type output) -> bool override;

			auto done(core::ModuleContext& context) -> void override;

			[[nodiscard]] constexpr auto size() const noexcept -> size_type { return size_; }
		};
	}
}
