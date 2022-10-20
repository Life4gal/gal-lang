#pragma once

#include <gsl/utils/memory_model.hpp>
#include <gsl/config.hpp>
#include <gsl/type/type.hpp>
#include <gsl/type/boolean.hpp>
#include <gsl/type/integer.hpp>
#include <gsl/type/floating_point.hpp>
#include <gsl/type/raw_string.hpp>
#include <gsl/type/wide.hpp>
#include <gsl/utils/assert.hpp>
#include <gsl/accelerate/string.hpp>

namespace gal::gsl::core
{
	class ModuleNode;

	class NodeAllocator : public accelerate::enable_shared_from_this<NodeAllocator>
	{
	public:
		using data_type = heap_data_type;
		using size_type = heap_data_size_type;

		// This structure must be able to be put into core::Value
		struct node_header
		{
			size_type size = 0;
			size_type magic = 0x11fe49al;

			size_type pad0{0};
			size_type pad1{0};
		};

	protected:
		utils::FixedChunkModel model_;

		size_type total_node_allocated_;
		bool node_header_required_;

	public:
		explicit NodeAllocator(const size_type initial_size = utils::FixedChunkModel::default_initial_size, const bool header_required = true)
			: model_{initial_size},
			total_node_allocated_{0},
			node_header_required_{true} {}

		NodeAllocator(const NodeAllocator& other) = delete;
		NodeAllocator(NodeAllocator&& other) noexcept = delete;
		auto operator=(const NodeAllocator& other) -> NodeAllocator& = delete;
		auto operator=(NodeAllocator&& other) noexcept -> NodeAllocator& = delete;

		virtual ~NodeAllocator() noexcept;

		template<typename Node, typename... Args>
		[[nodiscard]] auto allocate(Args&&... args) -> Node*
		{
			++total_node_allocated_;
			auto* ret = heap::allocate(sizeof(Node) + node_header_required_ * sizeof(node_header));

			if (node_header_required_)
			{
				std::construct_at(static_cast<node_header*>(ret));
				ret += sizeof(node_header);
			}

			return std::construct_at(static_cast<Node*>(ret), std::forward<Args>(args)...);
		}

		template<template<typename T> typename Node, typename... Args>
		[[nodiscard]] auto allocate_numeric(const type::Type type, Args&&... args) -> ModuleNode*
		{
			switch (type)
			{
				using enum type::Type;
				case SIGNED_INTEGER: { return this->allocate<Node<type::signed_integer>>(std::forward<Args>(args)...); }
				case UNSIGNED_INTEGER: { return this->allocate<Node<type::unsigned_integer>>(std::forward<Args>(args)...); }
				case SIGNED_LONG_INTEGER: { return this->allocate<Node<type::signed_long_integer>>(std::forward<Args>(args)...); }
				case UNSIGNED_LONG_INTEGER: { return this->allocate<Node<type::unsigned_long_integer>>(std::forward<Args>(args)...); }
				case SINGLE_PRECISION: { return this->allocate<Node<type::single_precision>>(std::forward<Args>(args)...); }
				case DOUBLE_PRECISION: { return this->allocate<Node<type::double_precision>>(std::forward<Args>(args)...); }

				// todo: more type?

				default:
				{
					gsl_trap("Required [numeric] type not supported! Add it first!");
					return nullptr;
				}
			}
		}

		template<template<typename T> typename Node, typename... Args>
		[[nodiscard]] auto allocate_value(const type::Type type, Args&&... args) -> ModuleNode*
		{
			switch (type)
			{
				using enum type::Type;
				case BOOLEAN: { return this->allocate<Node<type::boolean>>(std::forward<Args>(args)...); }
				case SIGNED_INTEGER: { return this->allocate<Node<type::signed_integer>>(std::forward<Args>(args)...); }
				case UNSIGNED_INTEGER: { return this->allocate<Node<type::unsigned_integer>>(std::forward<Args>(args)...); }
				case SIGNED_LONG_INTEGER: { return this->allocate<Node<type::signed_long_integer>>(std::forward<Args>(args)...); }
				case UNSIGNED_LONG_INTEGER: { return this->allocate<Node<type::unsigned_long_integer>>(std::forward<Args>(args)...); }
				case SINGLE_PRECISION: { return this->allocate<Node<type::single_precision>>(std::forward<Args>(args)...); }
				case DOUBLE_PRECISION: { return this->allocate<Node<type::double_precision>>(std::forward<Args>(args)...); }
				case RAW_STRING: { return this->allocate<Node<type::raw_string>>(std::forward<Args>(args)...); }
				case VEC4_FLOAT: { return this->allocate<Node<type::vec4f>>(std::forward<Args>(args)...); }
				case VEC4_SIGNED: { return this->allocate<Node<type::vec4i>>(std::forward<Args>(args)...); }
				case VEC4_UNSIGNED: { return this->allocate<Node<type::vec4u>>(std::forward<Args>(args)...); }

				// todo: more type?

				default:
				{
					gsl_trap("Required [construct-by-value] type not supported! Add it first!");
					return nullptr;
				}
			}
		}
	};

	class DebugNodeAllocator : public NodeAllocator
	{
	public:
		using hash_type = std::uint64_t;
		using name_type = accelerate::string;
		using name_view_type = accelerate::string_view;

		struct name_hasher
		{
			using is_transparent = int;

			[[nodiscard]] /* constexpr */ auto operator()(const name_type& name) const noexcept(std::is_nothrow_invocable_v<std::hash<name_type>, const name_type&>) -> std::size_t { return std::hash<name_type>{}(name); }

			[[nodiscard]] /* constexpr */ auto operator()(const name_view_type& name) const noexcept(std::is_nothrow_invocable_v<std::hash<name_view_type>, const name_view_type&>) -> std::size_t { return std::hash<name_view_type>{}(name); }
		};

	private:
		accelerate::unordered_map<name_type, data_type, name_hasher> named_cache_;
		size_type name_used_memory_;

	public:
		DebugNodeAllocator()
			: NodeAllocator{1024, false},
			name_used_memory_{0}
		{
			// DO NOT GROW
			model_.set_grow_function([](const auto& size) { return size; });
		}

		[[nodiscard]] auto allocate_name(name_view_type name) -> data_type;
	};
}
