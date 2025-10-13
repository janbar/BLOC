###############################################################################
# core library version must be set
if(NOT BLOC_LIB_SOVERSION)
  message(FATAL_ERROR "Core library version not set")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# build module "oracle"
file(GLOB bloc_oracle_SOURCES plugin_oracle.cpp)
file(GLOB bloc_oracle_HEADERS plugin_oracle.h)

add_subdirectory (oracle)
add_library (bloc_oracle SHARED ${bloc_oracle_SOURCES})
add_dependencies(bloc_oracle blocc odpi)
target_link_libraries(bloc_oracle blocc odpi)

set_target_properties(bloc_oracle PROPERTIES
  SOVERSION ${BLOC_LIB_SOVERSION}
)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_oracle LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_oracle CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
      RUNTIME DESTINATION bin)
  endif()

endif()
