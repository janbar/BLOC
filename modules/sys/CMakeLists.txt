###############################################################################
# core library version must be set
if(NOT BLOC_LIB_SOVERSION)
  message(FATAL_ERROR "Core library version not set")
endif()

# build module "sys"
file(GLOB bloc_sys_SOURCES plugin_sys.cpp)
file(GLOB bloc_sys_HEADERS plugin_sys.h)

add_library (bloc_sys SHARED ${bloc_sys_SOURCES})
add_dependencies(bloc_sys blocc)
target_link_libraries(bloc_sys blocc)

set_target_properties(bloc_sys PROPERTIES
  SOVERSION ${BLOC_LIB_SOVERSION}
)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_sys LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_sys CONFIGURATIONS Debug Release MinSizeRel
      RUNTIME DESTINATION bin)
  endif()

endif()
