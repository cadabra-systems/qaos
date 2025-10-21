QAOS_QT_MODULES = \
quickcontrols2 \
quick \
qml \
sql \
xml

QAOS_QT_RESOURCES = $$PWD/Qaos.qrc

QAOS_CXX_HEADERS = \
$$PWD/Qaos.hpp \
$$PWD/Date.hpp \
$$PWD/Range.hpp \
$$PWD/String.hpp \
$$PWD/Zip.hpp \
$$PWD/Default.hpp \
$$PWD/AtomicEnum.hpp \
$$PWD/NaturalSort.hpp \
$$PWD/MakeDateTime.hpp \
$$PWD/MakeUnique.hpp \
$$PWD/DAV/WebCollection.hpp \
$$PWD/DAV/WebConnection.hpp \
$$PWD/DAV/WebItem.hpp

QAOS_CXX_SOURCES = \
$$PWD/Qaos.cpp \
$$PWD/Date.cpp \
$$PWD/Range.cpp \
$$PWD/String.cpp \
$$PWD/Zip.cpp \
$$PWD/Default.cpp \
$$PWD/NaturalSort.cpp \
$$PWD/DAV/WebItem.cpp \
$$PWD/DAV/WebCollection.cpp \
$$PWD/DAV/WebConnection.cpp

contains(QT_PRIVATE, testlib) {
#QAOS_QT_RESOURCES += $$PWD/Test/QaosTest.qrc

#QAOS_CXX_HEADERS += $$files($$PWD/Test/*.hpp, true)
#QAOS_CXX_SOURCES += $$files($$PWD/Test/*.cpp, true)

#QAOS_CXX_HEADERS += $$files($$PWD/Shocker/*.hpp, true)
#QAOS_CXX_SOURCES += $$files($$PWD/Shocker/*.cpp, true)
}
