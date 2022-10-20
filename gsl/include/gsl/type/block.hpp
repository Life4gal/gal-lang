#pragma once

#include <gsl/core/value.hpp>
#include <gsl/type/type_descriptor.hpp>

namespace gal::gsl
{
	namespace core
	{
		class ModuleNode;
	}

	namespace type
	{
		class Block
		{
		public:
			using size_type = std::uint32_t;

		private:
			size_type stack_offset_;
			size_type arguments_offset_;
			core::ModuleNode* block_body_;
			core::Value* block_arguments_;
			core::FunctionInfo* block_info_;

		public:
			constexpr Block(
					const size_type stack_offset,
					const size_type arguments_offset,
					core::ModuleNode* block_body,
					core::Value* block_arguments,
					core::FunctionInfo* block_info
					)
				: stack_offset_{stack_offset},
				arguments_offset_{arguments_offset},
				block_body_{block_body},
				block_arguments_{block_arguments},
				block_info_{block_info} {}

			constexpr auto operator==(const Block& other) const noexcept -> bool
			{
				return stack_offset_ == other.stack_offset_ &&
						arguments_offset_ == other.arguments_offset_ &&
						block_body_ == other.block_body_ &&
						block_arguments_ == other.block_arguments_;
			}

			// todo
		};

		template<typename Return, typename... Args>
		class CallableBlock : public Block
		{
		public:
			using return_type = Return;
			using arguments_type = std::tuple<Args...>;

			using Block::Block;

			// todo
		};
	}

	namespace core
	{
		template<>
		class ValueCaster<type::Block> : public value_caster_undefined
		{
		public:
			constexpr static auto from(const type::Block& data) noexcept -> Value = delete;

			constexpr static auto to(const Value& data) noexcept -> type::Block = delete;
		};

		template<typename Return, typename... Args>
		class ValueCaster<type::CallableBlock<Return, Args...>> : public ValueCaster<type::Block> {};
	}// namespace core
}
