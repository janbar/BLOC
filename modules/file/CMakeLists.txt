###############################################################################
# core library version must be set
if(NOT BLOC_LIB_SOVERSION)
  message(FATAL_ERROR "Core library version not set")
endif()

# build module "file"
file(GLOB bloc_file_SOURCES plugin_file.cpp)
file(GLOB bloc_file_HEADERS plugin_file.h)

add_library (bloc_file SHARED ${bloc_file_SOURCES})
add_dependencies(bloc_file blocc)
target_link_libraries(bloc_file blocc)

set_target_properties(bloc_file PROPERTIES
  SOVERSION ${BLOC_LIB_SOVERSION}
)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_file LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_file CONFIGURATIONS Debug Release MinSizeRel
      RUNTIME DESTINATION bin)
  endif()

endif()
