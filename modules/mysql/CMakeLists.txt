###############################################################################
# core library version must be set
if(NOT BLOC_LIB_SOVERSION)
  message(FATAL_ERROR "Core library version not set")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# build module "mysql"
file(GLOB bloc_mysql_SOURCES plugin_mysql.cpp)
file(GLOB bloc_mysql_HEADERS plugin_mysql.h)

find_package(MySQL QUIET)
if(MYSQL_FOUND)
  message(STATUS "Found mysql ${MYSQL_INCLUDE_DIRS}")
  include_directories(${MYSQL_INCLUDE_DIRS})
  add_library (bloc_mysql SHARED ${bloc_mysql_SOURCES})
  add_dependencies(bloc_mysql blocc)
  target_link_libraries(bloc_mysql blocc ${MYSQL_LIBRARIES})

  set_target_properties(bloc_mysql PROPERTIES
    SOVERSION ${BLOC_LIB_SOVERSION}
  )
else()
  message(STATUS "Package mysql not found")
endif()

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL AND MYSQL_FOUND)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_mysql LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_mysql CONFIGURATIONS Debug
      RUNTIME DESTINATION Debug
      ARCHIVE DESTINATION Debug)
    install(TARGETS bloc_mysql CONFIGURATIONS Release
      RUNTIME DESTINATION Release
      ARCHIVE DESTINATION Release)
  endif()

endif()
