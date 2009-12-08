cmake_minimum_required(VERSION 2.6)
project(SNAssistant)
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

find_package(Qt4 REQUIRED)
find_package(Poppler REQUIRED)
find_package(Sqlite REQUIRED)
if(NOT SQLITE_FK_SUPPORT)
    message(FATAL_ERROR "The installed version of SQLite3 does not have foreign key support. Please upgrade to version >=3.6.19")
endif(NOT SQLITE_FK_SUPPORT)

include(${QT_USE_FILE})

set(CMAKE_CXX_FLAGS "-g -Wall")

#add_subdirectory(dialogs)
# add_subdirectory(delegates)
# add_subdirectory(models)
# add_subdirectory(views)

ENABLE_TESTING()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    dialogs
    data
    delegates
    models
    views
    widgets
    google_diff
    ${POPPLER_INCLUDE_DIR}
    ${QT_QTTEST_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTXML_INCLUDE_DIR}
    ${QT_QTSQL_INCLUDE_DIR}
    ${SQLITE_INCLUDE_DIR}
)

set( snass_SRCS
    #main.cpp
    Global.cpp
    MainWindow.cpp
    TurnParser.cpp
    data/SNItem.cpp
    data/ShipDesign.cpp
    data/Empire.cpp
    data/System.cpp
    data/Planet.cpp
    models/ShipDesignsModel.cpp
    models/ItemModel.cpp
    models/MaterialsModel.cpp
    models/ComponentsProxyModel.cpp
    models/ComponentsModel.cpp
    data/Order.cpp
    dialogs/ItemConflictDialog.cpp
    dialogs/NewEmpireDialog.cpp
    data/ItemEffect.cpp
    delegates/GenericDelegate.cpp
    delegates/IntegerColumnDelegate.cpp
    delegates/ItemDelegate.cpp
    tests/modeltest.cpp
    google_diff/diff_match_patch.cpp
    SNConstants.cpp
    widgets/WelcomeWidget.cpp
    widgets/ShipDesigner.cpp
    widgets/ItemBrowser.cpp
    widgets/MaterialsWidget.cpp
    widgets/SystemsOverviewWidget.cpp
)
qt4_automoc(${snass_SRCS})

# headers that need mocs generated
set( snass_TOMOC
    MainWindow.h
    models/ComponentsModel.h
    models/ShipDesignsModel.h
    models/ComponentsProxyModel.h
    models/ItemModel.h
    models/MaterialsModel.h
    dialogs/ItemConflictDialog.h
    dialogs/NewEmpireDialog.h
    delegates/GenericDelegate.h
    tests/modeltest.h
    widgets/WelcomeWidget.h
    widgets/ShipDesigner.h
    widgets/ItemBrowser.h
    widgets/SystemsOverviewWidget.h
    TurnParser.h
)

# run moc
qt4_wrap_cpp(moc_outfiles ${snass_TOMOC})

#files that need ui headers generated
set( snass_UIS
    MainWindow.ui
    dialogs/NewEmpireDialog.ui
    dialogs/ItemConflictDialog.ui
    widgets/WelcomeWidget.ui
    widgets/ShipDesigner.ui
    widgets/ItemBrowser.ui
    widgets/SystemsOverviewWidget.ui
)
# run uic
QT4_WRAP_UI(snass_UIS_H ${snass_UIS})

set( SN_RCS
    application.qrc 
    )


QT4_ADD_RESOURCES( SN_RC_SRCS ${SN_RCS} )

if(MINGW)
   ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/sn_ico.obj
                        COMMAND /opt/mingw32/bin/i586-pc-mingw32-windres
                            -I${CMAKE_CURRENT_SOURCE_DIR}
                            -o ${CMAKE_CURRENT_BINARY_DIR}/sn_ico.obj
                            -i${CMAKE_CURRENT_SOURCE_DIR}/sn.rc
                        )
    set(snass_SRCS ${snass_SRCS} sn_ico.obj)
endif(MINGW)

add_library( snassistant_lib STATIC ${snass_SRCS} ${snass_UIS_H} ${moc_outfiles} ${SN_RC_SRCS} )

target_link_libraries(snassistant_lib
    ${QT_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTXML_LIBRARIES}
    ${QT_QTSQL_LIBRARIES}
    ${QT_QTTEST_LIBRARIES}
    ${POPPLER_LIBRARY}
    ${SQLITE_LIBRARIES}
    )
SET( CMAKE_BUILD_TYPE debugfull )
if(WIN32)
 SET( CMAKE_CXX_FLAGS_DEBUG     "-O0 -g -Wall" )
 SET( CMAKE_CXX_FLAGS_DEBUGFULL "-O0 -g3 -fno-inline -Wall -Woverloaded-virtual -Wsign-compare -Wundef" )
 add_executable(SNAssistant WIN32 ${snass_SRCS} ${SN_RC_SRCS} main.cpp)
else(WIN32)
 add_executable(SNAssistant ${snass_SRCS} ${SN_RC_SRCS} main.cpp)
endif(WIN32)

IF(WIN32)
  target_link_libraries(SNAssistant ${QT_LIBRARIES} snassistant_lib "-Wl,-subsystem,console" )
else(WIN32)
  target_link_libraries(SNAssistant ${QT_LIBRARIES} snassistant_lib )
endif(WIN32)

add_subdirectory(tests)


