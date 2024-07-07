
file(GLOB bloc_SOURCES
  main.cpp
  main_options.cpp
  cli_parser.cpp
  read_file.cpp
)

if(UNIX)
  find_package(Readline QUIET)
  if(READLINE_FOUND)
   include_directories(${Readline_INCLUDE_DIR})
   add_definitions("-DHAVE_READLINE")
  endif()
  list(APPEND bloc_SOURCES signalhandler.cpp)
elseif(MSVC)
  list(APPEND bloc_SOURCES winstub.cpp)
endif()

add_executable(bloc ${bloc_SOURCES})
add_dependencies(bloc blocc)
target_link_libraries(bloc blocc ${blocc_LIBRARIRES})
if(READLINE_FOUND)
  target_link_libraries(bloc ${Readline_LIBRARY})
endif()

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  else()
    install(TARGETS bloc CONFIGURATIONS Debug
      RUNTIME DESTINATION Debug)
    install(TARGETS bloc CONFIGURATIONS Release
      RUNTIME DESTINATION Release)
  endif()

endif()
