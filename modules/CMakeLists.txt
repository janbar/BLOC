
add_subdirectory (file)
add_subdirectory (date)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  # Components PATH
  execute_process(
    COMMAND dpkg-architecture -qDEB_HOST_MULTIARCH
    OUTPUT_VARIABLE ARCH_TRIPLET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Set install paths
  include(GNUInstallDirs)

  if(NOT MSVC)
    install(TARGETS bloc_file LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(TARGETS bloc_date LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  endif()

endif()
