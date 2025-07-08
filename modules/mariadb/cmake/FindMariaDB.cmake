#
# Find the MariaDB client includes and library
#

# This module defines
# MARIADB_INCLUDE_DIRS, where to find mysql.h
# MARIADB_LIBRARIES, the libraries to link against to connect to MariaDB
# MARIADB_FOUND, If false, you cannot build anything that requires MariaDB

# also defined, but not for general use are
# MARIADB_LIBRARY, where to find the MariaDB library

#
# XXX NOTE: This is not yet for general use.  I'm pretty sure there
# are other libraries I have to link against at the same time.
#

SET( MARIADB_FOUND 0 )

FIND_PATH( MARIADB_INCLUDE_DIRS mysql.h
  PATH_SUFFIXES mariadb
  PATHS
  "C:/Program Files/MariaDB/include"
  "C:/MariaDB/include"
  DOC "Specify the directory containing mysql.h."
)

FIND_LIBRARY( MARIADB_LIBRARY
  NAMES mariadb mariadbclient libmariadb libmariadbclient
  PATHS
  "C:/Program Files/MiariaDB/lib"
  "C:/MariaDB/lib"
  DOC "Specify the mariadb library here."
)

# On Windows you typically don't need to include any extra libraries
# to build MariaDB stuff.

IF (NOT WIN32)
  FIND_LIBRARY( MARIADB_EXTRA_LIBRARIES
                NAMES z zlib
                PATHS /usr/lib /usr/local/lib /opt/local/lib
                DOC "If more libraries are necessary to link in a MariaDB client (typically zlib), specify them here.")
ELSE (NOT WIN32)
  SET( MARIADB_EXTRA_LIBRARIES "" )
ENDIF (NOT WIN32)


IF (MARIADB_LIBRARY)
  IF (MARIADB_INCLUDE_DIRS)
    SET( MARIADB_FOUND 1 )
    SET( MARIADB_LIBRARIES ${MARIADB_LIBRARY} ${MARIADB_EXTRA_LIBRARIES})
  ENDIF (MARIADB_INCLUDE_DIRS)
ENDIF (MARIADB_LIBRARY)

MARK_AS_ADVANCED( MARIADB_FOUND MARIADB_LIBRARY MARIADB_EXTRA_LIBRARIES MARIADB_INCLUDE_DIRS )
