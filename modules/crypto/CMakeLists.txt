###############################################################################
# core library version must be set
if(NOT BLOC_LIB_SOVERSION)
  message(FATAL_ERROR "Core library version not set")
endif()

# build module "crypto"
file(GLOB bloc_crypto_SOURCES plugin_crypto.cpp aes.cpp md5.cpp)
file(GLOB bloc_crypto_HEADERS plugin_crypto.h aes.h md5.h)

add_library (bloc_crypto SHARED ${bloc_crypto_SOURCES})
add_dependencies(bloc_crypto blocc)
target_link_libraries(bloc_crypto blocc)

set_target_properties(bloc_crypto PROPERTIES
  SOVERSION ${BLOC_LIB_SOVERSION}
)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_crypto LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_crypto CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
      RUNTIME DESTINATION bin)
  endif()

endif()

