# - Try to find Sqlite
# Once done this will define
#
#  SQLITE_FOUND - system has Sqlite
#  SQLITE_INCLUDE_DIR - the Sqlite include directory
#  SQLITE_LIBRARIES - Link these to use Sqlite
#  SQLITE_DEFINITIONS - Compiler switches required for using Sqlite
#  SQLITE_FK_SUPPORT - sqlite is >=version 3.6.19, and hence has Foreign Key Support
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


# Copyright (c) 2008, Gilles Caulier, <caulier.gilles@gmail.com>
# Copyright (c) 2009, Casey Link, <unnamedrambler@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
include(CheckCXXSourceRuns)
if ( SQLITE_INCLUDE_DIR AND SQLITE_LIBRARIES )
   # in cache already
   SET(Sqlite_FIND_QUIETLY TRUE)
endif ( SQLITE_INCLUDE_DIR AND SQLITE_LIBRARIES )

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
if( NOT WIN32 )
  find_package(PkgConfig)

  pkg_check_modules(PC_SQLITE sqlite3)

  set(SQLITE_DEFINITIONS ${PC_SQLITE_CFLAGS_OTHER})
endif( NOT WIN32 )

FIND_PATH(SQLITE_INCLUDE_DIR NAMES sqlite3.h
  PATHS
  ${PC_SQLITE_INCLUDEDIR}
  ${PC_SQLITE_INCLUDE_DIRS}
)

FIND_LIBRARY(SQLITE_LIBRARIES NAMES sqlite3
  PATHS
  ${PC_SQLITE_LIBDIR}
  ${PC_SQLITE_LIBRARY_DIRS}
)
set( CMAKE_REQUIRED_LIBRARIES ${SQLITE_LIBRARIES})
set( CMAKE_REQUIRED_INCLUDES ${SQLITE_INCLUDE_DIR})
set( CMAKE_REQUIRED_DEFINITIONS ${SQLITE_DEFINITIONS})
set(CMAKE_REQUIRED_FLAGS "")
CHECK_CXX_SOURCE_RUNS("
#include <sqlite3.h>
 int main()
 {
 #ifndef SQLITE_VERSION
 #error \"SQLite header version number not found\"
 #endif
 #if SQLITE_VERSION_NUMBER < 3006019
 #error \"SQLite header version 3006019 not found.\"
 #endif

 //Check if library version matches header version.
 int libver = sqlite3_libversion_number();
 if( libver != SQLITE_VERSION_NUMBER )
    return 1;
 return 0;
}" SQLITE_FK_SUPPORT)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sqlite DEFAULT_MSG SQLITE_INCLUDE_DIR SQLITE_LIBRARIES )

# show the SQLITE_INCLUDE_DIR and SQLITE_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(SQLITE_INCLUDE_DIR SQLITE_LIBRARIES )

