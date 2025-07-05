###############################################################################
# core library version must be set
if(NOT BLOC_LIB_SOVERSION)
  message(FATAL_ERROR "Core library version not set")
endif()

# build module "csv"
file(GLOB bloc_csv_SOURCES plugin_csv.cpp csvparser.cpp)
file(GLOB bloc_csv_HEADERS plugin_csv.h csvparser.h)

add_library (bloc_csv SHARED ${bloc_csv_SOURCES})
add_dependencies(bloc_csv blocc)
target_link_libraries(bloc_csv blocc)

set_target_properties(bloc_csv PROPERTIES
  SOVERSION ${BLOC_LIB_SOVERSION}
)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_csv LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_csv CONFIGURATIONS Debug Release MinSizeRel
      RUNTIME DESTINATION bin)
  endif()

endif()
