###############################################################################
# core library version must be set
if(NOT BLOC_LIB_SOVERSION)
  message(FATAL_ERROR "Core library version not set")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# build module "sqlite3"
file(GLOB bloc_sqlite3_SOURCES plugin_sqlite3.cpp)
file(GLOB bloc_sqlite3_HEADERS plugin_sqlite3.h)

find_package(SQLite3 QUIET)
if(SQLite3_FOUND)
  message(STATUS "Found sqlite3 ${SQLite3_INCLUDE_DIRS}")
  include_directories(${SQLite3_INCLUDE_DIRS})
  add_library (bloc_sqlite3 SHARED ${bloc_sqlite3_SOURCES})
  add_dependencies(bloc_sqlite3 blocc)
  target_link_libraries(bloc_sqlite3 blocc ${SQLite3_LIBRARIES})

  set_target_properties(bloc_sqlite3 PROPERTIES
    SOVERSION ${BLOC_LIB_SOVERSION}
  )
else()
  message(STATUS "Package sqlite3 not found")
endif()

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL AND SQLite3_FOUND)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_sqlite3 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_sqlite3 CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
      RUNTIME DESTINATION bin)
  endif()

endif()
