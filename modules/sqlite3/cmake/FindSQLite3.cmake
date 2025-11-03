#
# Find the SQLite3 client includes and library
#

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_SQLite3 QUIET sqlite3)
endif()

# Look for the necessary header
find_path(SQLite3_INCLUDE_DIR NAMES sqlite3.h
  HINTS
    ${PC_SQLite3_INCLUDE_DIRS}
)

# Look for the necessary library
find_library(SQLite3_LIBRARY NAMES sqlite3 sqlite
  HINTS
    ${PC_SQLite3_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLite3
    REQUIRED_VARS SQLite3_LIBRARY SQLite3_INCLUDE_DIR)

# Create the imported target
if(SQLite3_FOUND)
    set(SQLite3_INCLUDE_DIRS ${SQLite3_INCLUDE_DIR})
    set(SQLite3_LIBRARIES ${SQLite3_LIBRARY})
    if(NOT TARGET SQLite::SQLite3)
        add_library(SQLite::SQLite3 UNKNOWN IMPORTED)
        set_target_properties(SQLite::SQLite3 PROPERTIES
            IMPORTED_LOCATION             "${SQLite3_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${SQLite3_INCLUDE_DIR}")
    endif()
endif()

mark_as_advanced(
  SQLite3_FOUND
  SQLite3_INCLUDE_DIR
  SQLite3_LIBRARY
  SQLite3_INCLUDE_DIRS
  SQLite3_LIBRARIES
)
