###############################################################################
# core library version must be set
if(NOT BLOC_LIB_SOVERSION)
  message(FATAL_ERROR "Core library version not set")
endif()

# build module "utf8"
file(GLOB bloc_utf8_SOURCES plugin_utf8.cpp utf8helper.cpp utf8helper_charmap.cpp)
file(GLOB bloc_utf8_HEADERS plugin_utf8.h utf8helper.h utf8helper_charmap.h)

add_library (bloc_utf8 SHARED ${bloc_utf8_SOURCES})
add_dependencies(bloc_utf8 blocc)
target_link_libraries(bloc_utf8 blocc)

set_target_properties(bloc_utf8 PROPERTIES
  SOVERSION ${BLOC_LIB_SOVERSION}
)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_utf8 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_utf8 CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
      RUNTIME DESTINATION bin)
  endif()

endif()
