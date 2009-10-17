include_directories( ../ )
include(${QT_USE_FILE})
#
# TurnParser test
#

set( SN_TEST_RCS
    tests.qrc )

QT4_ADD_RESOURCES( SN_TEST_RC_SRCS ${SN_TEST_RCS} )

SET(turnparser_test_SRCS TurnParserTest.cpp)
SET(turnparser_test_MOC_H TurnParserTest.h)
qt4_automoc(${turnparser_test_SRCS})
QT4_WRAP_CPP(turnparser_test_MOC_SRCS ${turnparser_test_MOC_H})
ADD_EXECUTABLE(turnparser_test ${turnparser_test_SRCS} ${turnparser_test_MOC_SRCS} ${SN_TEST_RC_SRCS} )
TARGET_LINK_LIBRARIES(turnparser_test
    ${QT_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTXML_LIBRARIES}
    ${QT_QTSQL_LIBRARIES}
    ${QT_QTTEST_LIBRARIES}
    ${POPPLER_LIBRARY}
    snassistant_lib )
ADD_TEST(turnparser_test turnparser_test)
