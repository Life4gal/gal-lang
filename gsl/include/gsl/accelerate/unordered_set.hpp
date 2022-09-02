#pragma once

#include <unordered_set>

namespace gal::gsl::accelerate
{
	template<
		typename Value,
		typename Hasher = ::std::hash<Value>,
		typename KeyComparator = ::std::equal_to<void>>
	using unordered_set = ::std::unordered_set<Value, Hasher, KeyComparator, boost::gsl_allocator<Value>>;

	template<
		typename Value,
		typename Hasher = ::std::hash<Value>,
		typename KeyComparator = ::std::equal_to<void>>
	using unordered_multiset = ::std::unordered_multiset<Value, Hasher, KeyComparator, boost::gsl_allocator<Value>>;
}
