
# build module "file"
file(GLOB bloc_file_SOURCES import_file.cpp)
file(GLOB bloc_file_HEADERS import_file.h)

add_library (bloc_file SHARED ${bloc_file_SOURCES})
add_dependencies(bloc_file blocc)
target_link_libraries(bloc_file blocc)
