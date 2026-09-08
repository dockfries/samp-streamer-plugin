# CPACK_PRE_BUILD_SCRIPTS hook: runs after CPack staged the files and before
# the archive is produced. Strip directories contributed by third-party
# install() rules (zydis/zycore via sampgdk).

# The staged package content lives in
# <CPACK_TOPLEVEL_DIRECTORY>/<CPACK_PACKAGE_FILE_NAME>/.
set(_stage "${CPACK_TOPLEVEL_DIRECTORY}/${CPACK_PACKAGE_FILE_NAME}")
if(EXISTS "${_stage}")
  message(STATUS "cpack strip hook: stripping lib/include from ${_stage}")
  file(REMOVE_RECURSE "${_stage}/lib" "${_stage}/include")
endif()
