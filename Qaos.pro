TARGET = Qaos
TEMPLATE = lib

CONFIG += staticlib
CONFIG += sdk_no_version_check
CONFIG += c++11
CONFIG += c++1z
CONFIG += c++17
CONFIG += lrelease
CONFIG -= -std=gnu++11

QMAKE_CXXFLAGS += -std=c++17
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter -Wno-unused-private-field

include(Qaos.pri)
QT_PRIVATE += $$QAOS_QT_MODULES
HEADERS += $$QAOS_CXX_HEADERS
SOURCES += $$QAOS_CXX_SOURCES
RESOURCES += $$QAOS_QT_RESOURCES
