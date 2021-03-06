# option to set custom grok name space
#set(GROK_NAMESPACE "FOO")
cmake_minimum_required(VERSION 3.12)

if(NOT GROK_NAMESPACE)
  set(GROK_NAMESPACE "GROK")
  set(GROK_STANDALONE 1)
endif()
set(GROK_LIBRARY_NAME grokj2k)
set(GROK_PLUGIN_NAME grokj2k_plugin)

project(${GROK_NAMESPACE} )

# Do full dependency headers.
include_regular_expression("^.*$")

#-----------------------------------------------------------------------------
# GROK version number, useful for packaging and doxygen doc:
set(GROK_VERSION_MAJOR 7)
set(GROK_VERSION_MINOR 2)
set(GROK_VERSION_BUILD 0)
set(GROK_VERSION
  "${GROK_VERSION_MAJOR}.${GROK_VERSION_MINOR}.${GROK_VERSION_BUILD}")
set(PACKAGE_VERSION
  "${GROK_VERSION_MAJOR}.${GROK_VERSION_MINOR}.${GROK_VERSION_BUILD}")

# Because autotools does not support X.Y notation for SOVERSION, we have to use
# two numbering, one for the Grok version and one for Grok soversion
# version | soversion
#   7.0.0 |  1
if(NOT GROK_SOVERSION)
  set(GROK_SOVERSION 1)
endif(NOT GROK_SOVERSION)
set(GROK_LIBRARY_PROPERTIES
  VERSION   "${GROK_VERSION_MAJOR}.${GROK_VERSION_MINOR}.${GROK_VERSION_BUILD}"
  SOVERSION "${GROK_SOVERSION}"
)

# --------------------------------------------------------------------------
# Path to additional CMake modules
set(CMAKE_MODULE_PATH
    ${${GROK_NAMESPACE}_SOURCE_DIR}/cmake
    ${CMAKE_MODULE_PATH})

if(WIN32)
  if(NOT BORLAND)
    if(NOT CYGWIN)
      if(NOT MINGW)
        if(NOT ITK_ENABLE_VISUAL_STUDIO_DEPRECATED_C_WARNINGS)
          add_definitions(
            -D_CRT_FAR_MAPPINGS_NO_DEPRECATE
            -D_CRT_IS_WCTYPE_NO_DEPRECATE
            -D_CRT_MANAGED_FP_NO_DEPRECATE
            -D_CRT_NONSTDC_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE_GLOBALS
            -D_CRT_SETERRORMODE_BEEP_SLEEP_NO_DEPRECATE
            -D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
            -D_CRT_VCCLRIT_NO_DEPRECATE
            -D_SCL_SECURE_NO_DEPRECATE
            )
        endif()
      endif()
    endif()
  endif()
endif()


# --------------------------------------------------------------------------
# Install directories
include(GNUInstallDirs)
# Build DOCUMENTATION (not in ALL target and only if Doxygen is found)
option(BUILD_DOC "Build the HTML documentation (with doxygen if available)." OFF)

string(TOLOWER ${PROJECT_NAME} projectname)
set(GROK_INSTALL_SUBDIR "grok-${GROK_VERSION_MAJOR}.${GROK_VERSION_MINOR}")

if(NOT GROK_INSTALL_PACKAGE_DIR)
  set(GROK_INSTALL_PACKAGE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${GROK_INSTALL_SUBDIR}")
endif()

if (APPLE)
	list(APPEND GROK_LIBRARY_PROPERTIES INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}")
	option(GROK_USE_DSYMUTIL "Call dsymutil on binaries after build." OFF)
endif()

#-----------------------------------------------------------------------------
# Big endian test:
include (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(GROK_BIG_ENDIAN)

#-----------------------------------------------------------------------------
# Setup file for setting custom ctest vars
configure_file(
  ${${GROK_NAMESPACE}_SOURCE_DIR}/cmake/CTestCustom.cmake.in
  ${${GROK_NAMESPACE}_BINARY_DIR}/CTestCustom.cmake
  @ONLY
  )

#-----------------------------------------------------------------------------
# Grok build configuration options.
option(BUILD_SHARED_LIBS "Build Grok shared library and link executables against it." ON)
option(BUILD_STATIC_LIBS "Build Grok static library." ON)
set (EXECUTABLE_OUTPUT_PATH ${GROK_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")
set (LIBRARY_OUTPUT_PATH ${GROK_BINARY_DIR}/bin CACHE PATH "Single output directory for building all libraries.")
mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

#-----------------------------------------------------------------------------
# Compiler specific flags:
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  set(GROK_COMPILE_OPTIONS ${GROK_COMPILE_OPTIONS} -Wall -Wextra -Wconversion -Wunused-parameter)
endif()

find_package(SSE)

#-----------------------------------------------------------------------------
# grk_config.h generation (1/2)

# Option Choose whether to use static runtime
include(ucm)
if(BUILD_SHARED_LIBS)
    ucm_set_runtime(DYNAMIC)
else()
    ucm_set_runtime(STATIC)
endif()

# Check if some include files are provided by the system
include(EnsureFileInclude)
# These files are mandatory
ensure_file_include("string.h"   HAVE_STRING_H YES)
ensure_file_include("memory.h"   HAVE_MEMORY_H YES)
ensure_file_include("stdlib.h"   HAVE_STDLIB_H YES)
ensure_file_include("stdio.h"    HAVE_STDIO_H  YES)
ensure_file_include("math.h"     HAVE_MATH_H   YES)
ensure_file_include("float.h"    HAVE_FLOAT_H  YES)
ensure_file_include("time.h"     HAVE_TIME_H   YES)
ensure_file_include("stdarg.h"   HAVE_STDARG_H YES)
ensure_file_include("ctype.h"    HAVE_CTYPE_H  YES)
ensure_file_include("assert.h"   HAVE_ASSERT_H YES)

CHECK_INCLUDE_FILE("strings.h"      HAVE_STRINGS_H)
CHECK_INCLUDE_FILE("sys/stat.h"     HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE("sys/types.h"    HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE("unistd.h"       HAVE_UNISTD_H)

# Enable Large file support
include(TestLargeFiles)
GROK_TEST_LARGE_FILES(GROK_HAVE_LARGEFILES)

# Allocating Aligned Memory Blocks
include(CheckIncludeFiles)
check_include_files(malloc.h GROK_HAVE_MALLOC_H)
include(CheckSymbolExists)
# _aligned_alloc https://msdn.microsoft.com/en-us/library/8z34s9c6.aspx
check_symbol_exists(_aligned_malloc malloc.h GROK_HAVE__ALIGNED_MALLOC)
check_symbol_exists(aligned_alloc stdlib.h GROK_HAVE_ALIGNED_ALLOC)
# posix_memalign (needs _POSIX_C_SOURCE >= 200112L on Linux)
set(CMAKE_REQUIRED_DEFINITIONS -D_POSIX_C_SOURCE=200112L)
check_symbol_exists(posix_memalign stdlib.h GROK_HAVE_POSIX_MEMALIGN)
unset(CMAKE_REQUIRED_DEFINITIONS)
# memalign (obsolete)
check_symbol_exists(memalign malloc.h GROK_HAVE_MEMALIGN)
#-----------------------------------------------------------------------------
# Build Library
add_subdirectory(src/lib)
option(BUILD_LUTS_GENERATOR "Build utility to generate t1_luts.h" OFF)
option(BUILD_UNIT_TESTS "Build unit tests (bench_dwt, test_sparse_array, etc..)" OFF)

#-----------------------------------------------------------------------------
# Build Applications
option(BUILD_CODEC "Build the CODEC executables" ON)
option(BUILD_PLUGIN_LOADER "Enable loading of T1 plugin" OFF)
mark_as_advanced(BUILD_PLUGIN_LOADER)

if(BUILD_CODEC)
  # OFF: It will only build 3rd party libs if they are not found on the system
  # ON: 3rd party libs will ALWAYS be build, and used
  option(BUILD_THIRDPARTY "Build the thirdparty executables if it is needed" OFF)
  add_subdirectory(thirdparty)
  add_subdirectory(src/bin)
endif ()

#-----------------------------------------------------------------------------
# grk_config.h generation (2/2)
configure_file(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/jp2/grk_config.h.cmake.in
 ${CMAKE_CURRENT_BINARY_DIR}/src/lib/jp2/grk_config.h
 @ONLY
 )

 configure_file(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/jp2/grk_config_private.h.cmake.in
 ${CMAKE_CURRENT_BINARY_DIR}/src/lib/jp2/grk_config_private.h
 @ONLY
 )

#-----------------------------------------------------------------------------
# build documentation in doc subdir:
if(BUILD_DOC)
  add_subdirectory(doc)
endif()

#-----------------------------------------------------------------------------
# Build Testing
option(BUILD_TESTING "Build the tests." OFF)
if(BUILD_TESTING)
  if(BUILD_CODEC)
    enable_testing()
    include(CTest)

    # Search for Grok test data
    # The test data can be cloned from https://github.com/GrokImageCompression/grok-test-data
    find_path(GROK_DATA_ROOT README-GROK-TEST-DATA
      PATHS $ENV{GROK_DATA_ROOT} ${CMAKE_SOURCE_DIR}/../grok-test-data
      NO_CMAKE_FIND_ROOT_PATH
      )

    # Add repository where to find tests
    add_subdirectory(tests)

  else()
    message(FATAL_ERROR "You need build codec to run the tests")
  endif()
endif()

#-----------------------------------------------------------------------------
# install all targets referenced as GrokTargets
install(EXPORT GrokTargets DESTINATION ${GROK_INSTALL_PACKAGE_DIR})
configure_file( ${${GROK_NAMESPACE}_SOURCE_DIR}/cmake/GrokConfig.cmake.in
  ${${GROK_NAMESPACE}_BINARY_DIR}/GrokConfig.cmake
  @ONLY
)
install( FILES ${GROK_BINARY_DIR}/GrokConfig.cmake
  DESTINATION ${GROK_INSTALL_PACKAGE_DIR}
)

#-----------------------------------------------------------------------------
# install CHANGES and LICENSE
if(BUILD_DOC)
if(EXISTS ${GROK_SOURCE_DIR}/CHANGES)
  install(FILES CHANGES DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif()

install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif()

include (cmake/GrokCPack.cmake)

#-----------------------------------------------------------------------------
# pkgconfig support
# enabled by default on Unix, disabled by default on other platforms
if(UNIX)
  option(BUILD_PKGCONFIG_FILES "Build and install pkg-config files" ON)
else()
  option(BUILD_PKGCONFIG_FILES "Build and install pkg-config files" OFF)
endif()
if(BUILD_PKGCONFIG_FILES)
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/lib/jp2/libgrokj2k.pc.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/libgrokj2k.pc @ONLY)
  install( FILES  ${CMAKE_CURRENT_BINARY_DIR}/libgrokj2k.pc DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/pkgconfig )
endif()


# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
