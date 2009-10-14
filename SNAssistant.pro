# -------------------------------------------------
# Project created by QtCreator 2009-08-30T08:42:50
# -------------------------------------------------
TARGET = SNAssistant
TEMPLATE = app
UI_DIR = ui
OBJECTS_DIR = objs
INCLUDEPATH += dialogs
DEPENDPATH += dialogs
FORMS += MainWindow.ui \
    ShipDesigner.ui \
    dialogs/NewEmpireDialog.ui \
    dialogs/ChangeEmpireDialog.ui \
    dialogs/ItemBrowser.ui
SOURCES += main.cpp \
    MainWindow.cpp \
    ShipDesigner.cpp \
    models/ComponentsModel.cpp \
    data/SNItem.cpp \
    data/ShipDesign.cpp \
    dialogs/NewEmpireDialog.cpp \
    data/Empire.cpp \
    models/ShipDesignsModel.cpp \
    models/ItemModel.cpp \
    models/MaterialsModel.cpp \
    data/Order.cpp \
    dialogs/ChangeEmpireDialog.cpp \
    dialogs/ItemBrowser.cpp \
    data/ItemEffect.cpp \
    delegates/GenericDelegate.cpp \
    delegates/IntegerColumnDelegate.cpp \
    delegates/ItemDelegate.cpp \
    MaterialsWidget.cpp
HEADERS += MainWindow.h \
    models/ComponentsModel.h \
    ShipDesigner.h \
    data/SNItem.h \
    data/ShipDesign.h \
    dialogs/NewEmpireDialog.h \
    data/Empire.h \
    models/ShipDesignsModel.h \
    models/ItemModel.h \
    models/MaterialsModel.h \
    data/Order.h \
    dialogs/ChangeEmpireDialog.h \
    dialogs/ItemBrowser.h \
    data/ItemEffect.h \
    delegates/GenericDelegate.h \
    delegates/IntegerColumnDelegate.h \
    delegates/ItemDelegate.h \
    MaterialsWidget.h
QT += gui \
    xml \
    sql
RESOURCES += application.qrc
RC_FILE = sn.rc
ICON = icons/supernova.icns
