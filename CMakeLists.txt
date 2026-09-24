cmake_minimum_required(VERSION 3.12.4)

qt_add_library(
		Qaos STATIC

		${CMAKE_CURRENT_SOURCE_DIR}/AtomicEnum.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Date.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Date.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/DAV/WebCollection.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/DAV/WebCollection.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/DAV/WebConnection.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/DAV/WebConnection.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/DAV/WebItem.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/DAV/WebItem.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Default.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Default.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/DynamicRole.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/DynamicRole.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/FilesystemTask.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/FilesystemTask.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/LogRegister.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/LogRegister.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/MakeDateTime.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/MakeUnique.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/NaturalSort.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/NaturalSort.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/NetworkAccessManager.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/NetworkAccessManager.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/ScopedObjectConnection.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/ScopedObjectConnection.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/ObjectList.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/ObjectList.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/ObjectListModel.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/ObjectListModel.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/ObjectPathModel.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/ObjectPathModel.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/ObjectTreeModel.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/ObjectTreeModel.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PropertyList.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/PropertyList.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Qaos.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Qaos.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Range.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Range.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/SortFilterProxyModel.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/SortFilterProxyModel.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/String.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/String.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/ThreadTaskManager.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/ThreadTaskManager.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/VariantItemModel.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/VariantItemModel.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/QML.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/QML.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Zip.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Zip.cpp
)

target_include_directories(Qaos PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Qaos PUBLIC Qt6::Core)
target_link_libraries(Qaos PUBLIC Qt6::Gui)
target_link_libraries(Qaos PUBLIC Qt6::QuickControls2)
target_link_libraries(Qaos PUBLIC Qt6::Quick)
target_link_libraries(Qaos PUBLIC Qt6::Qml)
target_link_libraries(Qaos PUBLIC Qt6::Sql)
target_link_libraries(Qaos PUBLIC Qt6::Xml)

queen_add_qml_module(
			Qaos

			URI Qaos
			VERSION 0.1
)

add_subdirectory(QtQuick)
target_link_libraries(Qaos PUBLIC QaosQtQuick)
