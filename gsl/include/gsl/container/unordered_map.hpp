#pragma once

#include <unordered_map>
#include <gsl/memory/allocator.hpp>

namespace gal::gsl::container
{
	template<
		typename Key,
		typename Value,
		typename Hasher = std::hash<Key>,
		typename KeyComparator = std::equal_to<>>
	using unordered_map = std::unordered_map<
		Key,
		Value,
		Hasher,
		KeyComparator,
		memory::StlAllocator<std::pair<const Key, Value>>
	>;

	template<
		typename Key,
		typename Value,
		typename Hasher = std::hash<Key>,
		typename KeyComparator = std::equal_to<>>
	using unordered_multimap = std::unordered_multimap<
		Key,
		Value,
		Hasher,
		KeyComparator,
		memory::StlAllocator<std::pair<const Key, Value>>>;
}
