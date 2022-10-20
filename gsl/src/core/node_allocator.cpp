#include <gsl/core/node_allocator.hpp>

namespace gal::gsl::core
{
	NodeAllocator::~NodeAllocator() noexcept = default;

	auto DebugNodeAllocator::allocate_name(accelerate::string_view name) -> NodeAllocator::data_type
	{
		if (const auto it = named_cache_.find(name);
			it != named_cache_.end()) { return it->second; }

		auto* ret = model_.allocate(static_cast<size_type>(name.length() + 1));
		std::memcpy(ret, name.data(), name.length());
		ret[name.length()] = '\0';

		gsl_verify(named_cache_.emplace(name, ret).second);

		name_used_memory_ += decltype(model_)::policy_type::get_fit_aligned_size(static_cast<size_type>(name.length() + 1));

		return ret;
	}
}
