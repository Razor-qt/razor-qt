cmake_minimum_required( VERSION 2.6 )

set(DISTRO_MENU "")

# for now we have onlu SUSE related custom menu
if (EXISTS /etc/SuSE-release OR EXISTS /etc/SUSE-release OR EXISTS /etc/SuSEconfig)
    message(STATUS "SUSE Linux distro detected building additional menu structure")
    set(DISTRO_MENU "${DISTRO_MENU}<Menu><Name>YaST</Name><Directory>suse-yast.directory</Directory><MergeFile>yast-settings.menu</MergeFile></Menu>")
    # almost anything can be here... example:
    #set(DISTRO_MENU "${DISTRO_MENU}<Menu><Name>SUSE</Name><MergeFile>applications.menu</MergeFile></Menu>")

elseif(EXISTS /etc/debian_version)
    message(STATUS "Debian Linux distro detected building additional menu structure")
    set(DISTRO_MENU "${DISTRO_MENU}
        <Menu>
            <Name>Debian</Name>
            <MergeFile>debian-menu.menu</MergeFile>
            <Directory>Debian.directory</Directory>
        </Menu>"
    )
else()
    message(STATUS "Razor vanilla menu structure used")
endif () # linux distro detecton

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/razor-applications.menu.in
               ${CMAKE_CURRENT_BINARY_DIR}/razor-applications.menu
               @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/razor-applications.menu DESTINATION /etc/xdg/menus/)
install(DIRECTORY desktop-directories DESTINATION share/ PATTERN ".desktop")
