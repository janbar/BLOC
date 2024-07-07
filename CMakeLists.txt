cmake_minimum_required(VERSION 3.8.2)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
project (bloc CXX C)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if((NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "None") AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(MSVC)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
  include(MSVCRuntime)
  set(MSVC_DYNAMIC_RUNTIME 1)
  configure_msvc_runtime()
endif()

# set lib version here
set (BLOC_LIB_VERSION "2.2.3")
set (BLOC_LIB_SOVERSION 2)

add_subdirectory (blocc)
add_subdirectory (apps)
add_subdirectory (modules)

configure_file(
  "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY
)

# Configure uninstall
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake")

# tests
option(BUILD_TESTING "Make tests" OFF)
include(CTest)
if(BUILD_TESTING)
  add_subdirectory (tests)
endif()
