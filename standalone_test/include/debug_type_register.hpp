#include <cassert>
#include <map>
#include <string>
#include <string_view>

#define GSL_STRING_TYPE_INCLUDE <string>
#define GSL_STRING_TYPE std::string
#define GSL_STRING_VIEW_TYPE_INCLUDE <string_view>
#define GSL_STRING_VIEW_TYPE std::string_view

#define GSL_DEBUG_TYPE_REGISTER(id, name) debug_type_register::register_debug_type(id, name)

struct debug_type_register
{
	inline static std::map<std::uint32_t, GSL_STRING_TYPE> debug_type_names{};

	static void register_debug_type(std::uint32_t id, const char* name)
	{
		if (const auto it = debug_type_names.find(id);
			it != debug_type_names.end()) { assert(name == it->second); }
		else { debug_type_names.emplace_hint(it, id, name); }
	}
};
