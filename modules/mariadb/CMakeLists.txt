###############################################################################
# core library version must be set
if(NOT BLOC_LIB_SOVERSION)
  message(FATAL_ERROR "Core library version not set")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# build module "mariadb"
file(GLOB bloc_mariadb_SOURCES plugin_mariadb.cpp)
file(GLOB bloc_mariadb_HEADERS plugin_mariadb.h)

find_package(MariaDB QUIET)
if(MARIADB_FOUND)
  message(STATUS "Found mariadb ${MARIADB_INCLUDE_DIRS}")
  include_directories(${MARIADB_INCLUDE_DIRS})
  add_library (bloc_mariadb SHARED ${bloc_mariadb_SOURCES})
  add_dependencies(bloc_mariadb blocc)
  target_link_libraries(bloc_mariadb blocc ${MARIADB_LIBRARIES})

  set_target_properties(bloc_mariadb PROPERTIES
    SOVERSION ${BLOC_LIB_SOVERSION}
  )
else()
  message(STATUS "Package mariadb not found")
endif()

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL AND MARIADB_FOUND)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_mariadb LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_mariadb CONFIGURATIONS Debug Release MinSizeRel
      RUNTIME DESTINATION bin)
  endif()

endif()
