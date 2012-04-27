
set(qmlnotification_src
    notificationview.cpp
)

set(qmlnotification_moc
    notificationview.h
)

set(qmlnotification_rsc
    resources/resources.qrc
)

qt4_wrap_cpp(qmlnotification_moc_src ${qmlnotification_moc})
qt4_add_resources(qmlnotification_rsc_src ${qmlnotification_rsc})

add_library(qmlnotifications STATIC
    ${qmlnotification_src}
    ${qmlnotification_moc_src}
    ${qmlnotification_rsc_src}
)
