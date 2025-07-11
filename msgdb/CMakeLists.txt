###############################################################################
# core library version must be set
if(NOT BLOC_LIB_SOVERSION OR NOT BLOC_LIB_VERSION)
  message(FATAL_ERROR "Core library version not set")
endif()

add_definitions("-DLIB_DLL_EXPORTS")

file(GLOB bloc_msgdb_SOURCES msgdb.c)
file(GLOB bloc_msgdb_HEADERS msgdb.h msgdb_lang.h msgdb_common.h)

add_library (bloc_msgdb SHARED ${bloc_msgdb_SOURCES})

set_target_properties(bloc_msgdb PROPERTIES
  VERSION "${BLOC_LIB_VERSION}"
  SOVERSION "${BLOC_LIB_SOVERSION}"
)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_msgdb LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_msgdb CONFIGURATIONS Debug Release MinSizeRel
      RUNTIME DESTINATION bin)
  endif()

endif()
