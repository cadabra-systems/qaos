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
$$PWD/DAV/WebCollection.hpp \
$$PWD/DAV/WebConnection.hpp \
$$PWD/DAV/WebItem.hpp \
$$PWD/AtomicEnum.hpp \
$$PWD/MakeDateTime.hpp \
$$PWD/MakeUnique.hpp \
$$PWD/NaturalSort.hpp \
$$PWD/String.hpp \
$$PWD/Zip.hpp

QAOS_CXX_SOURCES = \
$$PWD/Qaos.cpp \
$$PWD/Date.cpp \
$$PWD/DAV/WebCollection.cpp \
$$PWD/DAV/WebConnection.cpp \
$$PWD/DAV/WebItem.cpp \
$$PWD/NaturalSort.cpp \
$$PWD/String.cpp \
$$PWD/Zip.cpp

contains(QT, testlib) {
	#QAOS_QT_RESOURCES += $$PWD/Test/QaosTest.qrc

	#QAOS_CXX_HEADERS += $$files($$PWD/Test/*.hpp, true)
	#QAOS_CXX_SOURCES += $$files($$PWD/Test/*.cpp, true)

	#QAOS_CXX_HEADERS += $$files($$PWD/Shocker/*.hpp, true)
	#QAOS_CXX_SOURCES += $$files($$PWD/Shocker/*.cpp, true)
}
