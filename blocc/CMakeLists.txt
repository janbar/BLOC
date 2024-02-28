
###############################################################################
# set lib version here
set (BLOCC_LIB_VERSION "2.0.9")
set (BLOCC_LIB_SOVERSION 2)

add_definitions(
  "-DLIBVERSION=\"${BLOCC_LIB_VERSION}\""
  "-DLIBSOVERSION=\"${BLOCC_LIB_SOVERSION}\""
  "-DLIB_DLL_EXPORTS"
)

file(GLOB blocc_SOURCES
  lex._tokenizer.c
  readstdin.c
  bloc_capi.cpp
  collection.cpp
  complex.cpp
  context.cpp
  debug.cpp
  exception_parse.cpp
  exception_runtime.cpp
  executable.cpp
  expression_boolean.cpp
  expression_builtin.cpp
  expression_complex_ctor.cpp
  expression_functor.cpp
  expression_integer.cpp
  expression_item.cpp
  expression_literal.cpp
  expression_member.cpp
  expression_numeric.cpp
  expression_variable.cpp
  functor_manager.cpp
  operator.cpp
  plugin.cpp
  plugin_manager.cpp
  parse_expression.cpp
  parser.cpp
  string_reader.cpp
  parse_statement.cpp
  statement_begin.cpp
  statement.cpp
  statement_do.cpp
  statement_end.cpp
  statement_for.cpp
  statement_forall.cpp
  statement_function.cpp
  statement_if.cpp
  statement_import.cpp
  statement_let.cpp
  statement_print.cpp
  statement_put.cpp
  statement_return.cpp
  statement_trace.cpp
  statement_while.cpp
  statement_raise.cpp
  symbol.cpp
  tuple.cpp
  tuple_decl.cpp
  value.cpp
  member/*.cpp
  operator/*.cpp
  builtin/*.cpp
)

file(GLOB blocc_PUBLIC_HEADERS
  bloc_capi.h
  collection.h
  complex.h
  context.h
  debug.h
  declspec.h
  exception.h
  exception_parse.h
  exception_runtime.h
  executable.h
  expression.h
  expression_builtin.h
  expression_member.h
  operator.h
  plugin.h
  plugin_interface.h
  plugin_manager.h
  intrinsic_type.h
  parser.h
  string_reader.h
  readstdin.h
  statement.h
  symbol.h
  template_stack.h
  token.h
  tokenizer.h
  tuple_decl.h
  tuple.h
  value.h
)

file(GLOB blocc_PRIVATE_HEADERS
  tokenizer.lex
  parse_expression.h
  parse_statement.h
  expression_boolean.h
  expression_complex_ctor.h
  expression_functor.h
  expression_integer.h
  expression_item.h
  expression_literal.h
  expression_numeric.h
  expression_variable.h
  functor_manager.h
  statement_begin.h
  statement_break.h
  statement_continue.h
  statement_do.h
  statement_end.h
  statement_for.h
  statement_forall.h
  statement_function.h
  statement_if.h
  statement_import.h
  statement_let.h
  statement_print.h
  statement_put.h
  statement_return.h
  statement_trace.h
  statement_while.h
  statement_raise.h
  template_dtor.h
  member/*.h
  operator/*.h
  builtin/*.h
)

add_custom_target(blocc_HEADERS SOURCES ${blocc_PRIVATE_HEADERS} ${blocc_PUBLIC_HEADERS})

if(MSVC)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)
  list(APPEND blocc_SOURCES win32/dlfcn.c)
  list(APPEND blocc_PRIVATE_HEADERS win32/dlfcn.h)
endif()

add_library(blocc SHARED ${blocc_SOURCES})

if (MSVC)
  target_link_libraries (blocc ws2_32)
else ()
  target_link_libraries (blocc m dl)
endif ()

set_target_properties(blocc PROPERTIES
  VERSION "${BLOCC_LIB_VERSION}"
  SOVERSION "${BLOCC_LIB_SOVERSION}"
)

# Support building as a subproject of a larger cmake project
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/blocc)
file(COPY ${blocc_PUBLIC_HEADERS}
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/blocc)

target_include_directories(blocc
  PRIVATE
    ${CMAKE_SOURCE_DIR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE blocc)

###############################################################################
# install targets
# Create pkg-conf file.
# (We use the same files as ./configure does, so we
#  have to defined the same variables used there).

if(NOT SKIP_INSTALL_ALL)

  # Set install paths
  include(GNUInstallDirs)

  # Add all targets to the build-tree export set
  export(TARGETS blocc FILE "${CMAKE_CURRENT_BINARY_DIR}/bloccTargets.cmake")

  if(NOT MSVC)
    set (prefix      "${CMAKE_INSTALL_PREFIX}")
    set (exec_prefix "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
    set (libdir      "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    set (includedir  "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
    configure_file (cmake/blocc.pc.in blocc.pc @ONLY)
    configure_file (cmake/bloccConfig.cmake.in bloccConfig.cmake @ONLY)
    configure_file (cmake/bloccConfigVersion.cmake.in bloccConfigVersion.cmake @ONLY)

    install(TARGETS blocc
      EXPORT bloccTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/blocc
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/blocc.pc
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

    # Install the bloccConfig.cmake and bloccConfigVersion.cmake
    set(INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/blocc)
    install(FILES
      ${CMAKE_CURRENT_BINARY_DIR}/bloccConfig.cmake
      ${CMAKE_CURRENT_BINARY_DIR}/bloccConfigVersion.cmake
      DESTINATION ${INSTALL_CMAKE_DIR} COMPONENT dev)

    # Install the export set for use with the install-tree
    install(EXPORT bloccTargets
      DESTINATION ${INSTALL_CMAKE_DIR} COMPONENT dev)
  else()
    install(TARGETS blocc CONFIGURATIONS Debug
      RUNTIME DESTINATION Debug
      ARCHIVE DESTINATION Debug
      LIBRARY DESTINATION Debug)
    install(TARGETS blocc CONFIGURATIONS Release
      RUNTIME DESTINATION Release
      ARCHIVE DESTINATION Release
      LIBRARY DESTINATION Release)
  endif()

endif()
