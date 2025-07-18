#
# Find the MySQL client includes and library
#

# This module defines
# MYSQL_INCLUDE_DIRS, where to find mysql.h
# MYSQL_LIBRARIES, the libraries to link against to connect to MySQL
# MYSQL_FOUND, If false, you cannot build anything that requires MySQL

# also defined, but not for general use are
# MYSQL_LIBRARY, where to find the MySQL library

#
# XXX NOTE: This is not yet for general use.  I'm pretty sure there
# are other libraries I have to link against at the same time.
#

SET( MYSQL_FOUND 0 )

FIND_PATH( MYSQL_INCLUDE_DIRS mysql.h
  PATH_SUFFIXES mysql
  PATHS
  "C:/Program Files/MySQL/include"
  "C:/MySQL/include"
  DOC "Specify the directory containing mysql.h."
)

FIND_LIBRARY( MYSQL_LIBRARY
  NAMES mysql mysqlclient libmysql libmysqlclient
  PATHS
  "C:/Program Files/MySQL/lib"
  "C:/MySQL/lib"
  DOC "Specify the mysql library here."
)

# On Windows you typically don't need to include any extra libraries
# to build MYSQL stuff.

IF (NOT WIN32)
  FIND_LIBRARY( MYSQL_EXTRA_LIBRARIES
                NAMES z zlib
                PATHS /usr/lib /usr/local/lib /opt/local/lib
                DOC "If more libraries are necessary to link in a MySQL client (typically zlib), specify them here.")
ELSE (NOT WIN32)
  SET( MYSQL_EXTRA_LIBRARIES "" )
ENDIF (NOT WIN32)


IF (MYSQL_LIBRARY)
  IF (MYSQL_INCLUDE_DIRS)
    SET( MYSQL_FOUND 1 )
    SET( MYSQL_LIBRARIES ${MYSQL_LIBRARY} ${MYSQL_EXTRA_LIBRARIES})
  ENDIF (MYSQL_INCLUDE_DIRS)
ENDIF (MYSQL_LIBRARY)

MARK_AS_ADVANCED( MYSQL_FOUND MYSQL_LIBRARY MYSQL_EXTRA_LIBRARIES MYSQL_INCLUDE_DIRS )
