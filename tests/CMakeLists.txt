
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

###############################################################################
# add definitions
if (MSVC)
  add_definitions ("/D_CRT_SECURE_NO_WARNINGS")
  set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /W3")
  set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /W3")
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3 /Od /RTC1 /EHsc /nologo")
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /Od /RTC1 /EHsc /nologo")
endif ()

###############################################################################
# configure
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
)

###############################################################################
# add unit tests
include(UnitTestProject)

unittest_project(NAME test_parse_constant SOURCES test_parse_constant.cpp TARGET blocc)
unittest_project(NAME test_operators_integer SOURCES test_operators_integer.cpp TARGET blocc)
unittest_project(NAME test_operators_numeric SOURCES test_operators_numeric.cpp TARGET blocc)
unittest_project(NAME test_operators_type_mixing SOURCES test_operators_type_mixing.cpp TARGET blocc)
unittest_project(NAME test_operators_boolean SOURCES test_operators_boolean.cpp TARGET blocc)
unittest_project(NAME test_operators_relational SOURCES test_operators_relational.cpp TARGET blocc)
unittest_project(NAME test_math_constant SOURCES test_math_constant.cpp TARGET blocc)
unittest_project(NAME test_tuple SOURCES test_tuple.cpp TARGET blocc)
unittest_project(NAME test_table SOURCES test_table.cpp TARGET blocc)
unittest_project(NAME test_math_function SOURCES test_math_function.cpp TARGET blocc)
unittest_project(NAME perf_hash SOURCES perf_hash.cpp TARGET blocc)
unittest_project(NAME perf_prim SOURCES perf_prim.cpp TARGET blocc)
unittest_project(NAME test_exception_handling SOURCES test_exception_handling.cpp TARGET blocc)

