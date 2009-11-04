#me of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)
 
# which compilers to use for C and C++
SET(CMAKE_C_COMPILER /opt/mingw32/bin/i586-pc-mingw32-gcc)
SET(CMAKE_CXX_COMPILER /opt/mingw32/bin/i586-pc-mingw32-g++)
 
# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  /opt/mingw32)
 
# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
 
# FindQt4.cmake querys qmake to get information, this doesn't work when crosscompiling
set(KDE_PREFIX    /opt/mingw32)
set(QT_BINARY_DIR   /opt/mingw32/bin)
set(QT_LIBRARY_DIR  ${KDE_PREFIX}/lib)
set(QT_QTCORE_LIBRARY   ${KDE_PREFIX}/lib/libQtCore4.a)
set(QT_INCLUDE_DIR  ${KDE_PREFIX}/include/qt4)
set(QT_QTCORE_INCLUDE_DIR ${KDE_PREFIX}/include/qt4/QtCore)
set(QT_MKSPECS_DIR  ${KDE_PREFIX}/share/qt4/mkspecs)
set(QT_MOC_EXECUTABLE  ${QT_BINARY_DIR}/moc.exe)
set(QT_QMAKE_EXECUTABLE  ${QT_BINARY_DIR}/qmake.exe)
set(QT_UIC_EXECUTABLE  ${QT_BINARY_DIR}/uic.exe)

set(QT_QTGUI_INCLUDE_DIR ${KDE_PREFIX}/include/qt4/QtGui)
set(QT_QTSQL_INCLUDE_DIR ${KDE_PREFIX}/include/qt4/QtSql)
set(QT_QTTEST_INCLUDE_DIR ${KDE_PREFIX}/include/qt4/QtTest)
set(QT_QTXML_INCLUDE_DIR ${KDE_PREFIX}/include/qt4/QtXml)
#set(Qt4_FIND_VERSION_EXACT FALSE)
#set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /opt/mingw32/lib)

set(CMAKE_RC_COMPILER_INIT /opt/mingw32/bin/i586-pc-mingw32-windres)
