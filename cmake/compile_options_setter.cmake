macro(set_compile_options_private target_name)
	message("Setting compile parameters for project ${target_name}...")
	target_compile_options(
		${target_name}
		PRIVATE

		### basic flag
		$<$<CXX_COMPILER_ID:MSVC>:/std:c++latest /W4 /WX>
		$<$<CXX_COMPILER_ID:GNU>:-std=c++2b -Wall -Wextra -Wpedantic -Werror>
		# clang-cl
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>:/std:c++latest /W4 /WX>
		# clang
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:-std=c++2b -Wall -Wextra -Wpedantic -Werror>
		### basic flag end

		### not debug
		$<$<AND:$<NOT:$<CONFIG:Debug>>,$<CXX_COMPILER_ID:MSVC>>:/DNDEBUG>
		$<$<AND:$<NOT:$<CONFIG:Debug>>,$<CXX_COMPILER_ID:GNU>>:-DNDEBUG>
		# clang-cl
		$<$<AND:$<NOT:$<CONFIG:Debug>>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/DNDEBUG>
		# clang
		$<$<AND:$<NOT:$<CONFIG:Debug>>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-DNDEBUG>
		### not debug end

		### extra flag
		
		# build type ==> Debug
		$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/MDd /Zi /Ob0 /Od /RTC1>
		$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:-O0 -g>
		# clang-cl
		$<$<AND:$<CONFIG:Debug>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/MDd /Zi /Ob0 /Od>
		# clang
		$<$<AND:$<CONFIG:Debug>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-O0 -g>

		# build type ==> Release
		$<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/MD /O2 /Ob2>
		$<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU>>:-O3>
		# clang-cl
		$<$<AND:$<CONFIG:Release>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/MD /O2 /Ob2>
		# clang
		$<$<AND:$<CONFIG:Release>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-O3>

		# build type ==> RelWithDebInfo
		$<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:MSVC>>:/MD /Zi /O2 /Ob1>
		$<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:GNU>>:-O2 -g>
		# clang-cl
		$<$<AND:$<CONFIG:RelWithDebInfo>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/MD /Zi /O2 /Ob1>
		# clang
		$<$<AND:$<CONFIG:RelWithDebInfo>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-O2 -g>

		# build type ==> MinSizeRel
		$<$<AND:$<CONFIG:MinSizeRel>,$<CXX_COMPILER_ID:MSVC>>:/MD /O1 /Ob1>
		$<$<AND:$<CONFIG:MinSizeRel>,$<CXX_COMPILER_ID:GNU>>:-Os>
		# clang-cl
		$<$<AND:$<CONFIG:MinSizeRel>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/MD /O1 /Ob1>
		# clang
		$<$<AND:$<CONFIG:MinSizeRel>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-Os>
		### extra flag end
	)
endmacro()

macro(set_compile_options_public target_name)
	message("Setting compile parameters for project ${target_name}...")
	target_compile_options(
		${target_name}
		PUBLIC

		### basic flag
		$<$<CXX_COMPILER_ID:MSVC>:/std:c++latest /W4 /WX>
		$<$<CXX_COMPILER_ID:GNU>:-std=c++2b -Wall -Wextra -Wpedantic -Werror>
		# clang-cl
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>:/std:c++latest /W4 /WX>
		# clang
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:-std=c++2b -Wall -Wextra -Wpedantic -Werror>
		### basic flag end

		### not debug
		$<$<AND:$<NOT:$<CONFIG:Debug>>,$<CXX_COMPILER_ID:MSVC>>:/DNDEBUG>
		$<$<AND:$<NOT:$<CONFIG:Debug>>,$<CXX_COMPILER_ID:GNU>>:-DNDEBUG>
		# clang-cl
		$<$<AND:$<NOT:$<CONFIG:Debug>>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/DNDEBUG>
		# clang
		$<$<AND:$<NOT:$<CONFIG:Debug>>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-DNDEBUG>
		### not debug end

		### extra flag

		# build type ==> Debug
		$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/MDd /Zi /Ob0 /Od /RTC1>
		$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:-O0 -g>
		# clang-cl
		$<$<AND:$<CONFIG:Debug>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/MDd /Zi /Ob0 /Od>
		# clang
		$<$<AND:$<CONFIG:Debug>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-O0 -g>

		# build type ==> Release
		$<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/MD /O2 /Ob2>
		$<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU>>:-O3>
		# clang-cl
		$<$<AND:$<CONFIG:Release>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/MD /O2 /Ob2>
		# clang
		$<$<AND:$<CONFIG:Release>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-O3>

		# build type ==> RelWithDebInfo
		$<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:MSVC>>:/MD /Zi /O2 /Ob1>
		$<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:GNU>>:-O2 -g>
		# clang-cl
		$<$<AND:$<CONFIG:RelWithDebInfo>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/MD /Zi /O2 /Ob1>
		# clang
		$<$<AND:$<CONFIG:RelWithDebInfo>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-O2 -g>

		# build type ==> MinSizeRel
		$<$<AND:$<CONFIG:MinSizeRel>,$<CXX_COMPILER_ID:MSVC>>:/MD /O1 /Ob1>
		$<$<AND:$<CONFIG:MinSizeRel>,$<CXX_COMPILER_ID:GNU>>:-Os>
		# clang-cl
		$<$<AND:$<CONFIG:MinSizeRel>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/MD /O1 /Ob1>
		# clang
		$<$<AND:$<CONFIG:MinSizeRel>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-Os>
		### extra flag end
	)
endmacro()

macro(set_compile_options_interface target_name)
	message("Setting compile parameters for project ${target_name}...")
	target_compile_options(
		${target_name}
		INTERFACE

		### basic flag
		$<$<CXX_COMPILER_ID:MSVC>:/std:c++latest /W4 /WX>
		$<$<CXX_COMPILER_ID:GNU>:-std=c++2b -Wall -Wextra -Wpedantic -Werror>
		# clang-cl
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>:/std:c++latest /W4 /WX>
		# clang
		$<$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:-std=c++2b -Wall -Wextra -Wpedantic -Werror>
		### basic flag end

		### not debug
		$<$<AND:$<NOT:$<CONFIG:Debug>>,$<CXX_COMPILER_ID:MSVC>>:/DNDEBUG>
		$<$<AND:$<NOT:$<CONFIG:Debug>>,$<CXX_COMPILER_ID:GNU>>:-DNDEBUG>
		# clang-cl
		$<$<AND:$<NOT:$<CONFIG:Debug>>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/DNDEBUG>
		# clang
		$<$<AND:$<NOT:$<CONFIG:Debug>>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-DNDEBUG>
		### not debug end

		### extra flag

		# build type ==> Debug
		$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/MDd /Zi /Ob0 /Od /RTC1>
		$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:-O0 -g>
		# clang-cl
		$<$<AND:$<CONFIG:Debug>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/MDd /Zi /Ob0 /Od>
		# clang
		$<$<AND:$<CONFIG:Debug>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-O0 -g>

		# build type ==> Release
		$<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/MD /O2 /Ob2>
		$<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU>>:-O3>
		# clang-cl
		$<$<AND:$<CONFIG:Release>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/MD /O2 /Ob2>
		# clang
		$<$<AND:$<CONFIG:Release>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-O3>

		# build type ==> RelWithDebInfo
		$<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:MSVC>>:/MD /Zi /O2 /Ob1>
		$<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:GNU>>:-O2 -g>
		# clang-cl
		$<$<AND:$<CONFIG:RelWithDebInfo>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/MD /Zi /O2 /Ob1>
		# clang
		$<$<AND:$<CONFIG:RelWithDebInfo>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-O2 -g>

		# build type ==> MinSizeRel
		$<$<AND:$<CONFIG:MinSizeRel>,$<CXX_COMPILER_ID:MSVC>>:/MD /O1 /Ob1>
		$<$<AND:$<CONFIG:MinSizeRel>,$<CXX_COMPILER_ID:GNU>>:-Os>
		# clang-cl
		$<$<AND:$<CONFIG:MinSizeRel>,$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>:/MD /O1 /Ob1>
		# clang
		$<$<AND:$<CONFIG:MinSizeRel>,$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<STREQUAL:"${CMAKE_CXX_SIMULATE_ID}","MSVC">>>>:-Os>
		### extra flag end
	)
endmacro()
