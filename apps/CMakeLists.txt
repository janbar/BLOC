
file(GLOB bloc_SOURCES main.cpp main_options.cpp cli_parser.cpp read_file.cpp)

add_executable(bloc ${bloc_SOURCES})
add_dependencies(bloc blocc)
target_link_libraries(bloc blocc ${blocc_LIBRARIRES})

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  # Set install paths
  include(GNUInstallDirs)

  if(NOT MSVC)
    install(TARGETS bloc RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  endif()

endif()
