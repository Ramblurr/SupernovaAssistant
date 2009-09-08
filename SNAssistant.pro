# -------------------------------------------------
# Project created by QtCreator 2009-08-30T08:42:50
# -------------------------------------------------
TARGET = SNAssistant
TEMPLATE = app
INCLUDEPATH += dialogs
DEPENDPATH += dialogs
FORMS += MainWindow.ui \
    ShipDesigner.ui \
    dialogs/NewEmpireDialog.ui \
    dialogs/ChangeEmpireDialog.ui
SOURCES += main.cpp \
    MainWindow.cpp \
    ShipDesigner.cpp \
    models/ComponentsModel.cpp \
    data/SNItem.cpp \
    data/ShipDesign.cpp \
    dialogs/NewEmpireDialog.cpp \
    data/Empire.cpp \
    models/ShipDesignsModel.cpp \
    models/EmpiresModel.cpp \
    models/ItemModel.cpp \
    models/MaterialsModel.cpp \
    data/Order.cpp \
    dialogs/ChangeEmpireDialog.cpp
HEADERS += MainWindow.h \
    models/ComponentsModel.h \
    ShipDesigner.h \
    data/SNItem.h \
    data/ShipDesign.h \
    dialogs/NewEmpireDialog.h \
    data/Empire.h \
    models/ShipDesignsModel.h \
    models/EmpiresModel.h \
    models/ItemModel.h \
    models/MaterialsModel.h \
    data/Order.h \
    dialogs/ChangeEmpireDialog.h
QT += gui \
    xml \
    sql
RESOURCES += application.qrc
LIBS += -lodbc"
UI_DIR = ui
