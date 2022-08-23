#pragma once

#include <unordered_map>

namespace gal::gsl::accelerate
{
	template<
		typename Key,
		typename Value,
		typename Hasher = ::std::hash<Key>,
		typename KeyComparator = ::std::equal_to<void>,
		typename Allocator = ::std::allocator<::std::pair<const Key, Value>>>
	using unordered_map = ::std::unordered_map<Key, Value, Hasher, KeyComparator, Allocator>;

	template<
		typename Key,
		typename Value,
		typename Hasher = ::std::hash<Key>,
		typename KeyComparator = ::std::equal_to<void>,
		typename Allocator = ::std::allocator<::std::pair<const Key, Value>>>
	using unordered_multimap = ::std::unordered_multimap<Key, Value, Hasher, KeyComparator, Allocator>;
}
