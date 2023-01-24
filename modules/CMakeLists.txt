
add_subdirectory (file)
add_subdirectory (date)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  # Set install paths
  include(GNUInstallDirs)

  if(NOT MSVC)
    install(TARGETS bloc_file LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(TARGETS bloc_date LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  endif()

endif()
