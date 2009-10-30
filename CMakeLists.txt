cmake_minimum_required(VERSION 2.6)
project(SNAssistant)
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

find_package(Qt4 REQUIRED)
find_package(Poppler REQUIRED)
find_package(Sqlite REQUIRED)

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
    ${POPPLER_INCLUDE_DIR}
    ${QT_QTTEST_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTXML_INCLUDE_DIR}
    ${QT_QTSQL_INCLUDE_DIR}
    ${SQLITE_INCLUDE_DIR}
)

set( snass_SRCS
    main.cpp
    MainWindow.cpp
    ShipDesigner.cpp
    TurnParser.cpp
    models/ComponentsModel.cpp
    data/SNItem.cpp
    data/ShipDesign.cpp
    dialogs/NewEmpireDialog.cpp
    data/Empire.cpp
    models/ShipDesignsModel.cpp
    models/ItemModel.cpp
    models/MaterialsModel.cpp
    models/ComponentsProxyModel.cpp
    data/Order.cpp
    dialogs/ChangeEmpireDialog.cpp
    dialogs/ItemBrowser.cpp
    data/ItemEffect.cpp
    delegates/GenericDelegate.cpp
    delegates/IntegerColumnDelegate.cpp
    delegates/ItemDelegate.cpp
    MaterialsWidget.cpp    
    tests/modeltest.cpp
)
qt4_automoc(${snass_SRCS})

# headers that need mocs generated
set( snass_TOMOC
    MainWindow.h
    ShipDesigner.h
    models/ComponentsModel.h
    dialogs/NewEmpireDialog.h
    models/ShipDesignsModel.h
    models/ComponentsProxyModel.h
    models/ItemModel.h
    models/MaterialsModel.h
    dialogs/ChangeEmpireDialog.h
    dialogs/ItemBrowser.h
    delegates/GenericDelegate.h
    tests/modeltest.h
)

# run moc
qt4_wrap_cpp(moc_outfiles ${snass_TOMOC})

#files that need ui headers generated
set( snass_UIS
    ShipDesigner.ui
    MainWindow.ui
    dialogs/ChangeEmpireDialog.ui
    dialogs/ItemBrowser.ui
    dialogs/NewEmpireDialog.ui
)
# run uic
QT4_WRAP_UI(snass_UIS_H ${snass_UIS})

set( SN_RCS
    application.qrc )

QT4_ADD_RESOURCES( SN_RC_SRCS ${SN_RCS} )

add_library( snassistant_lib SHARED ${snass_SRCS} ${snass_UIS_H} ${moc_outfiles} ${SN_RC_SRCS} )

target_link_libraries(snassistant_lib
    ${QT_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTXML_LIBRARIES}
    ${QT_QTSQL_LIBRARIES}
    ${QT_QTTEST_LIBRARIES}
    ${POPPLER_LIBRARY}
    ${SQLITE_LIBRARIES}
    )

add_executable(SNAssistant ${snass_SRCS} ${SN_RC_SRCS})

target_link_libraries(SNAssistant ${QT_LIBRARIES} snassistant_lib )

add_subdirectory(tests)


