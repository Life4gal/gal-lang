#include <type_traits>
#include <memory>

#if  !GAL_SCRIPT_LANG_COMPILER_HAS_META
#include<typeinfo>
#endif

namespace gal::gsl::core
{
	struct alignas(alignof(std::uint64_t)) type_info
	{
		using type_id_type = std::uint32_t;
		using attribute_type = std::uint32_t;

		type_id_type id;

		attribute_type is_void : 1 {0};
		attribute_type is_const : 1 {0};
		attribute_type is_reference : 1 {0};
		attribute_type is_pointer : 1 {0};
		attribute_type is_arithmetic : 1 {0};

		attribute_type is_trivially_default_constructible : 1 {0};
		attribute_type is_trivially_destructible : 1 {0};

		attribute_type is_trivially_copy_constructible : 1 {0};
		attribute_type is_trivially_copy_assignable : 1 {0};

		attribute_type is_trivially_move_constructible : 1 {0};
		attribute_type is_trivially_move_assignable : 1 {0};

		attribute_type is_ref_wrapped : 1 {0};
		attribute_type is_unique_ptr_wrapped : 1 {0};
		attribute_type is_shared_ptr_wrapped : 1 {0};
		attribute_type is_functional_wrapped : 1 {0};

		// todo
		attribute_type reserved : 17 {0};
	};

	static_assert(sizeof(type_info) == sizeof(std::uint64_t));
	// static_assert(alignof(type_info) == alignof(std::uint64_t));

	struct type_id_generator
	{
		template<typename T>
		constexpr /* static */ auto operator()() const noexcept
		{
			// FNV-1a hash. See: http://www.isthe.com/chongo/tech/comp/fnv/
			constexpr std::uint32_t hash_init{2166136261u};
			constexpr std::uint32_t hash_prime{16777619u};

			auto hash = hash_init;

			#if GAL_SCRIPT_LANG_COMPILER_HAS_META
			auto name = @type_string(T);
			#else
			auto name = typeid(T).name();
			#endif

			for (const auto* p = name; *p; ++p)
			{
				hash ^= *p;
				hash *= hash_prime;
			}

			GSL_DEBUG_TYPE_REGISTER(hash, name);
			return hash;
		}
	};

	template<typename T>
	constexpr type_info make_type_info() noexcept
	{
		// todo
		return {
			.id = type_id_generator{}.operator()<T>(),
		};
	}
}
