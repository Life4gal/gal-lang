#include <gsl/utils/allocator.hpp>

#include <gsl/utils/assert.hpp>
#include <gsl/boost/logger.hpp>

namespace
{
	#ifdef GSL_ALLOCATIONS_TRACK
	using id_type = gal::gsl::utils::allocator_tracker::id_type;

	id_type g_tracker = 0;
	id_type g_breakpoint = static_cast<id_type>(-1);
	#endif
}// namespace

namespace gal::gsl::utils
{
	#ifdef GSL_ALLOCATIONS_TRACK
	namespace allocator_tracker
	{
		[[nodiscard]] auto generate_id() noexcept -> id_type { return g_tracker++; }

		auto set_breakpoint(const id_type id) noexcept -> void { g_breakpoint = id; }

		auto freeze_if_required() noexcept -> void { if (g_tracker == g_breakpoint) { debug::freeze_here(); } }
	}// namespace allocator_tracker
	#endif

	AllocatorBase::~AllocatorBase() = default;

	auto StringAllocatorBase::construct(value_type string) -> StringAllocatorBase::value_type
	{
		if (string.empty()) { return {}; }

		if (intern_required_)
		{
			if (const auto it = intern_list_.find(string);
				it != intern_list_.end()) { return *it; }
		}

		if (auto* new_string = allocate(static_cast<AllocatorBase::size_type>(string.length() + 1)))
		{
			#ifdef GSL_ALLOCATIONS_TRACK
			allocator_tracker::freeze_if_required();
			#endif

			std::ranges::copy(string, new_string);
			new_string[string.length()] = '\0';

			if (intern_required_)
			{
				const auto [it, inserted] = intern_list_.insert(value_type{new_string, string.length()});
				if (!inserted) { boost::logger::warn("Construct a new string `{}`, but find that the same string already exists when intern.", string); }
				return *it;
			}
			return {new_string, string.length()};
		}

		return {};
	}

	auto StringAllocatorBase::destruct(value_type string) -> void
	{
		if (intern_required_)
		{
			if (const auto result = intern_list_.erase(string);
				result == 0) { boost::logger::warn("Destruct a string `{}`, but cannot find that the same string in list when intern.", string); }
		}

		deallocate(const_cast<data_type>(string.data()), static_cast<AllocatorBase::size_type>(string.length()));
	}

	auto StringAllocatorBase::intern(const value_type string) -> value_type
	{
		if (intern_required_)
		{
			if (const auto it = intern_list_.find(string);
				it != intern_list_.end()) { return *it; }
		}

		return {};
	}

	auto StringAllocatorBase::recognize(value_type string) -> void
	{
		if (string.empty()) { return; }

		// + 1 --> '\0'
		if (const auto real_size = policy_type::get_fit_aligned_size(static_cast<policy_type::size_type>(string.size() + 1));
			intern_required_ && inside(const_cast<data_type>(string.data()), real_size)) { intern_list_.insert(string); }
	}

	auto ConstStringAllocator::construct(value_type string) -> value_type
	{
		if (string.empty()) { return {}; }

		if (const auto it = intern_list_.find(string);
			it != intern_list_.end()) { return *it; }

		const auto ret = pool_.append(string);
		gsl_verify(intern_list_.insert(ret).second, "Impossible happened!");
		return ret;
	}

	auto ConstStringAllocator::intern(const value_type string) -> value_type
	{
		if (const auto it = intern_list_.find(string);
			it != intern_list_.end()) { return *it; }

		return {};
	}

	auto PersistentStringAllocator::sweep() -> void
	{
		model_.sweep();
		if (intern_required_)
		{
			StringAllocatorBase::reset();

			for_each([&](const value_type string) { gsl_verify(intern_list_.insert(string).second); });
		}
	}

	auto FixedChunkAllocator::peak_memory() const noexcept -> size_type
	{
		gsl_trap("NOT SUPPORTED!");
		return static_cast<size_type>(-1);
	}


	auto FixedChunkAllocator::mark(data_type data, size_type size) -> void { gsl_trap("NOT SUPPORTED!"); }

	auto FixedChunkAllocator::sweep() -> void { gsl_trap("NOT SUPPORTED!"); }
}
