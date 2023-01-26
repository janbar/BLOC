
add_subdirectory (file)
add_subdirectory (date)
add_subdirectory (utf8)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_file LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(TARGETS bloc_date LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(TARGETS bloc_utf8 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_file CONFIGURATIONS Debug
      RUNTIME DESTINATION Debug
      ARCHIVE DESTINATION Debug)
    install(TARGETS bloc_file CONFIGURATIONS Release
      RUNTIME DESTINATION Release
      ARCHIVE DESTINATION Release)
    install(TARGETS bloc_date CONFIGURATIONS Debug
      RUNTIME DESTINATION Debug
      ARCHIVE DESTINATION Debug)
    install(TARGETS bloc_date CONFIGURATIONS Release
      RUNTIME DESTINATION Release
      ARCHIVE DESTINATION Release)
    install(TARGETS bloc_utf8 CONFIGURATIONS Debug
      RUNTIME DESTINATION Debug
      ARCHIVE DESTINATION Debug)
    install(TARGETS bloc_utf8 CONFIGURATIONS Release
      RUNTIME DESTINATION Release
      ARCHIVE DESTINATION Release)
  endif()

endif()
