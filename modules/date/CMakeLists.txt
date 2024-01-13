
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include (CheckFunctionExists)
include (CheckFunctionKeywords)

check_function_exists (timegm HAVE_TIMEGM)
if(HAVE_TIMEGM)
  add_definitions("-DHAVE_TIMEGM")
endif()
check_function_exists (localtime_r HAVE_LOCALTIME_R)
if(HAVE_LOCALTIME_R)
  add_definitions("-DHAVE_LOCALTIME_R")
endif()
check_function_exists (gmtime_r HAVE_GMTIME_R)
if(HAVE_GMTIME_R)
  add_definitions("-DHAVE_GMTIME_R")
endif()

# Check what the inline keyword is.
check_function_keywords ("inline")
check_function_keywords ("__inline")
check_function_keywords ("__inline__")
if(HAVE_INLINE)
   set(CC_INLINE inline)
elseif(HAVE___INLINE)
   set(CC_INLINE __inline)
elseif(HAVE___INLINE__)
   set(CC_INLINE __inline__)
endif ()
if(CC_INLINE)
  add_definitions("-DCC_INLINE=${CC_INLINE}")
endif()

# build module "date"
file(GLOB bloc_date_SOURCES
  plugin_date.cpp
  nl_langinfo.c
  builtin.c
)

file(GLOB bloc_date_HEADERS
  plugin_date.h
  langinfo.h
  builtin.h
)

add_library (bloc_date SHARED ${bloc_date_SOURCES})
add_dependencies(bloc_date blocc)
target_link_libraries(bloc_date blocc)

set_target_properties(bloc_date PROPERTIES
  SOVERSION "2"
)

###############################################################################
# install targets

if(NOT SKIP_INSTALL_ALL)

  if(NOT MSVC)
    # Set install paths
    include(GNUInstallDirs)
    install(TARGETS bloc_date LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  else()
    install(TARGETS bloc_date CONFIGURATIONS Debug
      RUNTIME DESTINATION Debug
      ARCHIVE DESTINATION Debug)
    install(TARGETS bloc_date CONFIGURATIONS Release
      RUNTIME DESTINATION Release
      ARCHIVE DESTINATION Release)
  endif()

endif()
