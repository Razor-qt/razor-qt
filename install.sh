#!/bin/sh

green='\e[0;32m'
yellow='\e[0;33m'
white='\e[0;37m'

echo -e "${white}"

if [[ $EZUD -ne 0 ]]; then
  echo "Please run this as root or via sudo!" 1>&2
  exit 1
fi

if [ ! -e ${PWD}/razor ]; then
  echo -e "Please build binary first! (as user: qmake && make)" 1>&2
  exit 1
fi


if [ ! -e /usr/bin/razor ]; then
    cp ${PWD}/razor /usr/bin/razor
    echo -e "${green}Binary installed!" 
fi

if [ -d /etc/X11/sessions ]; then
    cp razor.desktop /etc/X11/sessions/razor-gdm.desktop
fi

if [ -d /usr/share/xsessions ]; then
    cp razor-kdm.desktop /usr/share/xsessions/razor-kdm.desktop
fi

if [ -d /usr/share/apps/kdm/sessions ]; then
    cp razor-kdm.desktop /usr/share/apps/kdm/sessions/razor-kdm.desktop
fi

echo -e "${white}***********************************************"
echo -e "${green}Razor ${yellow} successfully installed ${white}"
echo -e "***********************************************"
