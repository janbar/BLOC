
add_definitions(
  "-DLIBVERSION=\"${BLOC_LIB_VERSION}\""
)

file(GLOB bloc_SOURCES
  main.cpp
  main_options.cpp
  cli_parser.cpp
  cli_msgdb.cpp
  read_file.cpp
)

if(UNIX)
  option(BUILD_WITH_READLINE "Enable Readline support" ON)
  if (BUILD_WITH_READLINE)
    find_package(Readline QUIET)
    if(Readline_FOUND)
      include_directories(${Readline_INCLUDE_DIRS})
      add_definitions("-DHAVE_READLINE")
      message(STATUS "Found readline ${Readline_INCLUDE_DIRS}")
    else()
      message(STATUS "Package readline not found")
    endif()
  endif()
  list(APPEND bloc_SOURCES signalhandler.cpp)
elseif(MSVC)
  list(APPEND bloc_SOURCES "winstub.cpp" "win32/dlfcn.c")
endif()

add_executable(bloc ${bloc_SOURCES})
add_dependencies(bloc blocc)
target_link_libraries(bloc blocc ${blocc_LIBRARIES})
if(Readline_FOUND)
  target_link_libraries(bloc ${Readline_LIBRARIES})
endif()

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  else()
    install(TARGETS bloc CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
      RUNTIME DESTINATION bin)
    install(FILES ${PROJECT_SOURCE_DIR}/LICENSE
      DESTINATION bin)
  endif()

endif()
