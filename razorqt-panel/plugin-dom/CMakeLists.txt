cmake_minimum_required(VERSION 2.6)

set(PLUGIN "dom")

set(HEADERS
    domplugin.h
    treewindow.h
    domtreeitem.h
)

set(SOURCES
    domplugin.cpp
    treewindow.cpp
    domtreeitem.cpp
)

set(MOCS
    domplugin.h
    treewindow.h
    domtreeitem.h
)

set(UIS
    treewindow.ui
)

set(RESOURCES
    resources.qrc
)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
