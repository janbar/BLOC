
add_subdirectory (file)
add_subdirectory (date)
add_subdirectory (utf8)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  # Set install paths
  include(GNUInstallDirs)

  if(NOT MSVC)
    install(TARGETS bloc_file LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(TARGETS bloc_date LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(TARGETS bloc_utf8 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  endif()

endif()
