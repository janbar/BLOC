
# build module "utf8"
file(GLOB bloc_utf8_SOURCES import_utf8.cpp utf8helper.cpp utf8helper_charmap.cpp)
file(GLOB bloc_utf8_HEADERS import_utf8.h utf8helper.h utf8helper_charmap.h)

add_library (bloc_utf8 SHARED ${bloc_utf8_SOURCES})
add_dependencies(bloc_utf8 blocc)
target_link_libraries(bloc_utf8 blocc)

set_target_properties(bloc_utf8 PROPERTIES
  SOVERSION "1"
)
