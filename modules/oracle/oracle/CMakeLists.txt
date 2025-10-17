cmake_minimum_required(VERSION 3.8.2...3.20)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
project (odpi CXX C)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

if((NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "None") AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# set lib version here
set (ODPI_LIB_VERSION "5.3.0")
set (ODPI_LIB_SOVERSION 5)

# build module "odpi"
file(GLOB odpi_SOURCES
  odpi/src/*.c
)

file(GLOB odpi_HEADERS
  odpi/src/*.h
)

file(GLOB odpi_PUBLIC_HEADERS
  odpi/include/*.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/odpi/include)
add_library (odpi STATIC ${odpi_SOURCES})
set_property(TARGET odpi PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(odpi
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/odpi/src
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/odpi/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

