###############################################################################
# core library version must be set
if(NOT BLOC_LIB_SOVERSION)
  message(FATAL_ERROR "Core library version not set")
endif()

# build module "regex"
file(GLOB bloc_regex_SOURCES plugin_regex.cpp)
file(GLOB bloc_regex_HEADERS plugin_regex.h)

add_library (bloc_regex SHARED ${bloc_regex_SOURCES})
add_dependencies(bloc_regex blocc)
target_link_libraries(bloc_regex blocc)

set_target_properties(bloc_regex PROPERTIES
  SOVERSION ${BLOC_LIB_SOVERSION}
)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_regex LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_regex CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
      RUNTIME DESTINATION bin)
  endif()

endif()
