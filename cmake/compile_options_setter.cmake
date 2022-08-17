macro(set_compile_options_private target_name)
	message("Setting compile parameters for project ${target_name}...")
	target_compile_options(
		${target_name}
		PRIVATE

		$<$<CXX_COMPILER_ID:MSVC>:/std:c++latest /W4 /WX>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-std=c++2b -Wall -Wextra -Wpedantic -Werror>

		$<$<NOT:$<CONFIG:Debug>>: $<$<CXX_COMPILER_ID:MSVC>:/DNDEBUG> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-DNDEBUG>>

		$<$<CONFIG:Debug>: $<$<CXX_COMPILER_ID:MSVC>:/MDd /Zi /Ob0 /Od /RTC1> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O0 -g>>
		$<$<CONFIG:Release>: $<$<CXX_COMPILER_ID:MSVC>:/MD /O2 /Ob2> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3>>
		$<$<CONFIG:RelWithDebInfo>: $<$<CXX_COMPILER_ID:MSVC>:/MD /Zi /O2 /Ob1> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O2 -g>>
		$<$<CONFIG:MinSizeRel>: $<$<CXX_COMPILER_ID:MSVC>:/MD /O1 /Ob1> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Os>>
	)
endmacro()

macro(set_compile_options_public target_name)
	message("Setting compile parameters for project ${target_name}...")
	target_compile_options(
		${target_name}
		PUBLIC

		$<$<CXX_COMPILER_ID:MSVC>:/std:c++latest /W4 /WX>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-std=c++2b -Wall -Wextra -Wpedantic -Werror>

		$<$<NOT:$<CONFIG:Debug>>: $<$<CXX_COMPILER_ID:MSVC>:/DNDEBUG> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-DNDEBUG>>

		$<$<CONFIG:Debug>: $<$<CXX_COMPILER_ID:MSVC>:/MDd /Zi /Ob0 /Od /RTC1> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O0 -g>>
		$<$<CONFIG:Release>: $<$<CXX_COMPILER_ID:MSVC>:/MD /O2 /Ob2> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3>>
		$<$<CONFIG:RelWithDebInfo>: $<$<CXX_COMPILER_ID:MSVC>:/MD /Zi /O2 /Ob1> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O2 -g>>
		$<$<CONFIG:MinSizeRel>: $<$<CXX_COMPILER_ID:MSVC>:/MD /O1 /Ob1> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Os>>
	)
endmacro()

macro(set_compile_options_interface target_name)
	message("Setting compile parameters for project ${target_name}...")
	target_compile_options(
		${target_name}
		INTERFACE

		$<$<CXX_COMPILER_ID:MSVC>:/std:c++latest /W4 /WX>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-std=c++2b -Wall -Wextra -Wpedantic -Werror>

		$<$<NOT:$<CONFIG:Debug>>: $<$<CXX_COMPILER_ID:MSVC>:/DNDEBUG> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-DNDEBUG>>

		$<$<CONFIG:Debug>: $<$<CXX_COMPILER_ID:MSVC>:/MDd /Zi /Ob0 /Od /RTC1> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O0 -g>>
		$<$<CONFIG:Release>: $<$<CXX_COMPILER_ID:MSVC>:/MD /O2 /Ob2> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3>>
		$<$<CONFIG:RelWithDebInfo>: $<$<CXX_COMPILER_ID:MSVC>:/MD /Zi /O2 /Ob1> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O2 -g>>
		$<$<CONFIG:MinSizeRel>: $<$<CXX_COMPILER_ID:MSVC>:/MD /O1 /Ob1> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Os>>
	)
endmacro()
