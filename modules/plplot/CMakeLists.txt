###############################################################################
# core library version must be set
if(NOT BLOC_LIB_SOVERSION)
  message(FATAL_ERROR "Core library version not set")
endif()

# build module "plplot"
file(GLOB bloc_plplot_SOURCES plugin_plplot.cpp)
file(GLOB bloc_plplot_HEADERS plugin_plplot.h)

include(FindPLplot.cmake)
if(PLplot_FOUND)
  include_directories(${PLplot_INCLUDE_DIR}/..)
  add_library (bloc_plplot SHARED ${bloc_plplot_SOURCES})
  add_dependencies(bloc_plplot blocc)
  target_link_libraries(bloc_plplot blocc ${PLplot_cxx_LIBRARY})

  set_target_properties(bloc_plplot PROPERTIES
    SOVERSION ${BLOC_LIB_SOVERSION}
  )
else()
  message(STATUS "Package plplot not found")
endif()

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL AND PLplot_FOUND)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_plplot LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_plplot CONFIGURATIONS Debug Release MinSizeRel
      RUNTIME DESTINATION bin)
  endif()

endif()
