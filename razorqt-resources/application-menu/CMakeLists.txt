cmake_minimum_required( VERSION 2.6 )

install(FILES razor-applications.menu DESTINATION /etc/xdg/menus/)
install(DIRECTORY desktop-directories DESTINATION share/ PATTERN ".desktop")


