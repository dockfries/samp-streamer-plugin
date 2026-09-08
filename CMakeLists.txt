cmake_minimum_required(VERSION 3.5)
project(streamer)

set(PLUGIN_VERSION "2.9.6")
set(CMAKE_CXX_STANDARD 17)

option(SAMPGDK_ARCH "Target architecture (32 or 64)" 32)

if (MSVC)
    add_compile_options(/MP)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/lib/cmake-modules")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/lib/eigen/cmake")

set(SAMP_SDK_ROOT "${PROJECT_SOURCE_DIR}/lib/samp-plugin-sdk")
find_package(SAMPSDK REQUIRED)
include_directories(${SAMPSDK_DIR})
include_directories(${SAMPSDK_DIR}/amx)

include_directories(${PROJECT_SOURCE_DIR}/lib/boost)

if(EIGEN3_INCLUDE_DIR)
	file(TO_CMAKE_PATH ${EIGEN3_INCLUDE_DIR} EIGEN3_INCLUDE_DIR)
	list(APPEND CMAKE_MODULE_PATH "${EIGEN3_INCLUDE_DIR}/cmake")
else()
	set(EIGEN3_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/lib/eigen")
endif()

find_package(Eigen3 3.3.4 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY True)
set(CMAKE_SKIP_PACKAGE_ALL_DEPENDENCY True)

# sampgdk (and its Zydis/Zycore sub-libraries) are built as separate CMake
# targets that do not go through the add_samp_plugin macro, so they would not
# receive the -m32/-m64 flags applied there. Set the architecture globally on
# UNIX so every target (including the sampgdk subdirectory) is compiled for
# the same word size as the plugin module.
if(UNIX AND NOT MSVC)
	if(SAMPGDK_ARCH EQUAL 64)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
	endif()
endif()

# sampgdk is consumed as a git submodule (lib/sampgdk). Build it as a static,
# full library (the streamer calls sampgdk's IDL native wrappers directly,
# which the tiny build does not provide) and link it into the plugin module.
set(SAMPGDK_STATIC ON CACHE BOOL "" FORCE)
set(SAMPGDK_TINY OFF CACHE BOOL "" FORCE)
set(SAMPGDK_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(SAMPGDK_BUILD_PLUGINS OFF CACHE BOOL "" FORCE)
set(SAMP_SDK_ROOT "${PROJECT_SOURCE_DIR}/lib/samp-plugin-sdk")

add_subdirectory(lib/sampgdk sampgdk)

add_subdirectory(src)

set(CPACK_PACKAGE_VERSION ${PLUGIN_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Streamer plugin for SA-MP/open.mp")
set(CPACK_PACKAGE_DESCRIPTION "The streamer plugin provides high-performance object, pickup, checkpoint, race checkpoint, map icon, 3D text label, area, and actor streaming for SA-MP and open.mp servers.")

if(COMMIT_ID)
	string(TIMESTAMP CURRENT_DATE "%Y%m%d")
	set(PACKAGE_FILE_NAME streamer-${PLUGIN_VERSION}-${CURRENT_DATE}-${COMMIT_ID})
else()
	set(PACKAGE_FILE_NAME samp-streamer-plugin-${PLUGIN_VERSION})
endif()

set(CPACK_OUTPUT_FILE_PREFIX package/)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	set(PACKAGE_BUILD_TYPE debug)
else()
	set(PACKAGE_BUILD_TYPE release)
endif()

if(WIN32)
	if(SAMPGDK_ARCH EQUAL 64)
		set(CPACK_PACKAGE_FILE_NAME ${PACKAGE_FILE_NAME}-${PACKAGE_BUILD_TYPE}-win64)
	else()
		set(CPACK_PACKAGE_FILE_NAME ${PACKAGE_FILE_NAME}-${PACKAGE_BUILD_TYPE}-win32)
	endif()
	set(CPACK_GENERATOR ZIP)
else()
	if(SAMPGDK_ARCH EQUAL 64)
		set(CPACK_PACKAGE_FILE_NAME ${PACKAGE_FILE_NAME}-${PACKAGE_BUILD_TYPE}-linux-x86_64)
	else()
		set(CPACK_PACKAGE_FILE_NAME ${PACKAGE_FILE_NAME}-${PACKAGE_BUILD_TYPE}-linux-i386)
	endif()
	set(CPACK_GENERATOR TGZ)
endif()

if(CPACK_OUTPUT_FILE_POSTFIX)
	string(TOLOWER "${CPACK_OUTPUT_FILE_POSTFIX}" CPACK_OUTPUT_FILE_POSTFIX)
	set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}-${CPACK_OUTPUT_FILE_POSTFIX})
endif()

# Third-party deps pulled in via add_subdirectory (sampgdk -> zydis/zycore)
# register install() rules that land in lib/ and include/ of the staging
# area. Strip them after staging and before packaging (CPACK_PRE_BUILD_SCRIPTS
# runs post-install, pre-archive) so the archive only ships the plugin.
set(CPACK_PRE_BUILD_SCRIPTS ${PROJECT_SOURCE_DIR}/cmake/cpack_strip.cmake)

include(CPack)
