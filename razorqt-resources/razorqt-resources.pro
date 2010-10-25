TEMPLATE=subdirs
CONFIG += copy_dir_files
# this installs our ressource


desklink1.path = /etc/X11/sessions
desklink1.files = sys/razor-gdm.desktop

desklink2.path = /usr/share/xsessions  
desklink2.files = sys/razor-kdm.desktop

desklink3.path = /usr/share/apps/kdm/sessions
desklink3.files = sys/razor-kdm.desktop

usrshare.path = /usr/share/razor
usrshare.files = res/usrshare/* 


 linux-g++ | linux-g++-32  | linux-g++-64{
     message(Linux)
     message(Preparing install in /usr/)
     

INSTALLS += usrshare

exists( /etc/X11/sessions )
{
        INSTALLS += desklink1
}
exists( /usr/share/xsessions )
{
        INSTALLS += desklink2
}

exists( /usr/share/apps/kdm/sessions )
{
        INSTALLS += desklink3
}




 }

freebsd-g++ | netbsd-g++ | openbsd-g++ {
     message(*BSD)
     message(Preparing install in /usr/local/)

usrshare.path=/usr/local/share/razor
desklink1.path=/usr/local/etc/X11/sessions
desklink2.path=/usr/local/share/xsessions
desklink3.path=/usr/local/share/kde4/apps/kdm/sessions

bsdpatch.files=res/bsd-patch/*
bsdpatch.path=/usr/local/share/razor





INSTALLS += usrshare
INSTALLS += bsdpatch


exists( /usr/local/etc/X11/sessions )
{
        INSTALLS += desklink1
}
exists( /usr/local/share/xsessions )
{
        INSTALLS += desklink2
}

exists( /usr/local/share/kde4/apps/kdm/sessions )
{
        INSTALLS += desklink3
}





}


