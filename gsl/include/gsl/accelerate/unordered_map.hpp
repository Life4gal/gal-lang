#pragma once

#include <unordered_map>
#include <gsl/boost/allocator.hpp>

namespace gal::gsl::accelerate
{
	template<
		typename Key,
		typename Value,
		typename Hasher = ::std::hash<Key>,
		typename KeyComparator = ::std::equal_to<void>>
	using unordered_map = ::std::unordered_map<Key, Value, Hasher, KeyComparator, boost::gsl_allocator<::std::pair<const Key, Value>>>;

	template<
		typename Key,
		typename Value,
		typename Hasher = ::std::hash<Key>,
		typename KeyComparator = ::std::equal_to<void>>
	using unordered_multimap = ::std::unordered_multimap<Key, Value, Hasher, KeyComparator, boost::gsl_allocator<::std::pair<const Key, Value>>>;
}
