
# build module "file"
file(GLOB bloc_file_SOURCES import_file.cpp)
file(GLOB bloc_file_HEADERS import_file.h)

add_library (bloc_file SHARED ${bloc_file_SOURCES})
add_dependencies(bloc_file blocc)
target_link_libraries(bloc_file blocc)

set_target_properties(bloc_file PROPERTIES
  SOVERSION "1"
)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_file LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_file CONFIGURATIONS Debug
      RUNTIME DESTINATION Debug
      ARCHIVE DESTINATION Debug)
    install(TARGETS bloc_file CONFIGURATIONS Release
      RUNTIME DESTINATION Release
      ARCHIVE DESTINATION Release)
  endif()

endif()
