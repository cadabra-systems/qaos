cmake_minimum_required(VERSION 3.5)

list(APPEND CADABRA_QAOS_QRC
    ${CMAKE_CURRENT_LIST_DIR}/Qaos.qrc
)

list(APPEND CADABRA_QAOS_CPP
    ${CMAKE_CURRENT_LIST_DIR}/Qaos.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Zip.cpp
    ${CMAKE_CURRENT_LIST_DIR}/String.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NaturalSort.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Date.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MakeDateTime.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MakeUnique.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DAV/WebCollection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DAV/WebConnection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DAV/WebItem.cpp
)

list(APPEND CADABRA_QAOS_HPP
    ${CMAKE_CURRENT_LIST_DIR}/Qaos.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Zip.hpp
    ${CMAKE_CURRENT_LIST_DIR}/String.hpp
    ${CMAKE_CURRENT_LIST_DIR}/NaturalSort.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Date.hpp
    ${CMAKE_CURRENT_LIST_DIR}/MakeDateTime.hpp
    ${CMAKE_CURRENT_LIST_DIR}/MakeUnique.hpp
    ${CMAKE_CURRENT_LIST_DIR}/DAV/WebCollection.hpp
    ${CMAKE_CURRENT_LIST_DIR}/DAV/WebConnection.hpp
    ${CMAKE_CURRENT_LIST_DIR}/DAV/WebItem.hpp
)