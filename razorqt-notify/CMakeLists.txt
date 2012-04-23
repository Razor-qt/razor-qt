set(PROJECT razor-notificationd)
project(${PROJECT})

set(QT_USE_QTDECLARATIVE 1)
set(QT_USE_QTDBUS 1)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

add_subdirectory(src)
