###############################################################################
# core library version must be set
if(NOT BLOC_LIB_SOVERSION)
  message(FATAL_ERROR "Core library version not set")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# build module "postgresql"
file(GLOB bloc_postgresql_SOURCES plugin_postgresql.cpp)
file(GLOB bloc_postgresql_HEADERS plugin_postgresql.h)

find_package(PostgreSQL QUIET)
if(PostgreSQL_FOUND)
  message(STATUS "Found postgresql ${PostgreSQL_INCLUDE_DIRS}")
  include_directories(${PostgreSQL_INCLUDE_DIRS})
  add_library (bloc_postgresql SHARED ${bloc_postgresql_SOURCES})
  add_dependencies(bloc_postgresql blocc)
  target_link_libraries(bloc_postgresql blocc ${PostgreSQL_LIBRARIES})

  set_target_properties(bloc_postgresql PROPERTIES
    SOVERSION ${BLOC_LIB_SOVERSION}
  )
else()
  message(STATUS "Package postgresql not found")
endif()

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL AND PostgreSQL_FOUND)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_postgresql LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_postgresql CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
      RUNTIME DESTINATION bin)
  endif()

endif()
