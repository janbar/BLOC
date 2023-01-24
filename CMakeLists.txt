set(CMAKE_LEGACY_CYGWIN_WIN32 0)
project (bloc CXX C)
cmake_minimum_required(VERSION 3.8.2)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(MSVC)
  include(MSVCRuntime)
  set(MSVC_DYNAMIC_RUNTIME 1)
  configure_msvc_runtime()
endif()

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
