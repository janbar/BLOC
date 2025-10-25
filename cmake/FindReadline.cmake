# - Try to find readline include dirs and libraries 
#
# Usage of this module as follows:
#
#     find_package(Readline)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  Readline_ROOT_DIR         Set this variable to the root installation of
#                            readline if the module has problems finding the
#                            proper installation path.
#
# Variables defined by this module:
#
#  Readline_FOUND            System has readline, include and lib dirs found
#  Readline_INCLUDE_DIRS     The readline include directories. 
#  Readline_LIBRARIES        The readline libraries.

if(NOT Readline_FOUND)
    find_path(Readline_ROOT_DIR
        NAMES include/readline/readline.h
    )

    find_path(Readline_INCLUDE_DIR
        NAMES readline/readline.h
        HINTS ${Readline_ROOT_DIR}/include
    )

    find_library(Readline_LIBRARY
        NAMES readline
        HINTS ${Readline_ROOT_DIR}/lib
    )

    include(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(Readline DEFAULT_MSG Readline_INCLUDE_DIR Readline_LIBRARY)

    if(Readline_FOUND)
        set(Readline_LIBRARIES ${Readline_LIBRARY})
        set(Readline_INCLUDE_DIRS ${Readline_INCLUDE_DIR})
        if(${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")
            find_package(Curses QUIET)
            if(Curses_FOUND)
                list(APPEND Readline_LIBRARIES ${CURSES_LIBRARIES})
            else()
                set(Readline_FOUND FALSE)
            endif()
        endif()
    endif()

    MARK_AS_ADVANCED(Readline_INCLUDE_DIR Readline_LIBRARY)
endif()
