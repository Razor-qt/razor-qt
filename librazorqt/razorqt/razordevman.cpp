#ifndef RAZORDEVMAN_CPP
#define RAZORDEVMAN_CPP

#include "razordevman.h"

RazorDevMan::RazorDevMan ( QObject* parent ) : QObject ( parent )
{
}

RazorDev::RazorDev ( QObject* parent ) : QObject ( parent )
{

}

QString RazorDev::getMountPoint()
{
  return "";
}


QString RazorDev::getblockDev()
{
    return "";
}

QString RazorDev::getdriveType()
{
    return "";
}

QString RazorDev::getfsType()
{
    return "";
}

RazorDev* RazorDevMan::getDevice ( QString _uuid )
{
    return NULL;
}



bool RazorDev::getMounted()
{
    return false;
}


QString RazorDev::getuuid()
{
    return "";
}

QString RazorDev::getvolume()
{
    return "";
}

bool RazorDev::mount()
{
    return false;
}

bool RazorDev::umount()
{
    return false;
}

QList< QString > RazorDevMan::listDevices()
{
    QList < QString> tmp;
    return tmp;
}

bool RazorDevMan::mount ( QString _uuid )
{
    return false;
}

bool RazorDevMan::umount ( QString _uuid )
{
    return false;
}




#endif
