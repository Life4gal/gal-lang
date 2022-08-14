#!/bin/bash

CONFIG_BUILD_TYPE_LIST=("Debug" "Release" "RelWithDebugInfo" "MinSizeRel")
CONFIG_BUILD_TYPE_LIST_STRING=$(IFS=$'/'; echo "${CONFIG_BUILD_TYPE_LIST[*]}")

CONFIG_BUILD_TYPE="Debug"

# check build type
if [[ $# -eq 1 ]]; then
	# todo: ignore case?
	if [[ "${CONFIG_BUILD_TYPE_LIST[*]}" =~ "$1" ]]; then
		echo "Build type is set to $1."
		CONFIG_BUILD_TYPE=$1
	else
		echo "Fatal error: Unrecognized build type \`$1\`, supported build type is \`$CONFIG_BUILD_TYPE_LIST_STRING\`."
		exit -1
	fi
else
	echo "Build type not set, use default build type($CONFIG_BUILD_TYPE)."
fi

CONFIG_OUTPUT_FILENAME="CMakeSettings.json"
CONFIG_NAME="WSL_$CONFIG_BUILD_TYPE"
CONFIG_GENERATOR_NAME="Ninja"
CONFIG_BUILD_ROOT="\${projectDir}\\\\$CONFIG_NAME\\\\build"
CONFIG_INSTALL_ROOT="\${projectDir}\\\\$CONFIG_NAME\\\\install"
CONFIG_CMAKE_EXECUTABLE="cmake"
CONFIG_BUILD_COMMAND_ARGS=""
CONFIG_CTEST_COMMAND_ARGS=""
CONFIG_INHERIT_ENVIRONMENTS="linux_x64"
CONFIG_WSL_PATH="\${defaultWSLPath}"

# check cmake
if [[ -z "$(type -p cmake)" ]]; then
	echo "Fatal error: Unable to find CMake location! Install it first!"
	exit -1
fi

# check ninja
if [[ -z "$(type -p ninja)" ]]; then
	echo "Fatal error: Unable to find Ninja-build location! Install it first!"
	exit -1
fi

# check c/cxx compiler
CONFIG_CXX_COMPILER_LOC="$(type -p circle)"
CONFIG_C_COMPILER_LOC="$(type -p gcc)"
CONFIG_CXX_COMPILER_HAS_META="true"

if [[ -z $CONFIG_CXX_COMPILER_LOC ]]; then
	# currently only circle supports META.:(
	CONFIG_CXX_COMPILER_HAS_META="false"
	CONFIG_CXX_COMPILER_LOC="$(type -p "$g++")"
elif [[ -z $CONFIG_CXX_COMPILER_LOC ]]; then
	CONFIG_CXX_COMPILER_LOC="$(type -p "$clang++")"
fi

if [[ -z $CONFIG_C_COMPILER_LOC ]]; then
	CONFIG_C_COMPILER_LOC="$(type -p "$clang")"
fi

if [[ -z $CONFIG_CXX_COMPILER_LOC ]]; then
	echo "Fatal error: Unable to find C++ compiler location!"
	exit -1
else
	echo "Found C++ compiler at '$CONFIG_CXX_COMPILER_LOC'"
fi

if [[ -z $CONFIG_C_COMPILER_LOC ]]; then
	echo "Fatal error: Unable to find C compiler location!"
	exit -1
else
	echo "Found C compiler at '$CONFIG_C_COMPILER_LOC'"
fi

CONFIG_JSON_STRING="{
	\"configurations\": 
		[
			{
				\"name\": \"$CONFIG_NAME\",
				\"generator\": \"$CONFIG_GENERATOR_NAME\",
				\"configurationType\": \"$CONFIG_BUILD_TYPE\",
				\"buildRoot\": \"$CONFIG_BUILD_ROOT\",
				\"installRoot\": \"$CONFIG_INSTALL_ROOT\",
				\"cmakeExecutable\": \"$CONFIG_CMAKE_EXECUTABLE\",
				\"cmakeCommandArgs\": \"-G \\\"$CONFIG_GENERATOR_NAME\\\" -DCMAKE_C_COMPILER=$CONFIG_C_COMPILER_LOC -DCMAKE_CXX_COMPILER=$CONFIG_CXX_COMPILER_LOC -DCONFIG_CXX_COMPILER_HAS_META=$CONFIG_CXX_COMPILER_HAS_META \",
				\"buildCommandArgs\": \"$CONFIG_BUILD_COMMAND_ARGS\",
				\"ctestCommandArgs\": \"$CONFIG_CTEST_COMMAND_ARGS\",
				\"inheritEnvironments\": [\"$CONFIG_INHERIT_ENVIRONMENTS\"],
				\"wslPath\": \"$CONFIG_WSL_PATH\"
			}
		]
}"

echo "Writing configuration file to $CONFIG_OUTPUT_FILENAME..."
echo "$CONFIG_JSON_STRING" >| $CONFIG_OUTPUT_FILENAME
