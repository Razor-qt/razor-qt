#ifndef RAZORDEVMAN_CPP
#define RAZORDEVMAN_CPP

#include "razordevman.h"

Razordevman::Razordevman ( QObject* parent ) : QObject ( parent )
{
}

Razordev::Razordev ( QObject* parent ) : QObject ( parent )
{

}

QString Razordev::getblockDev()
{
    return "";
}

QString Razordev::getdriveType()
{
    return "";
}

QString Razordev::getfsType()
{
    return "";
}

Razordev* Razordevman::getDevice ( QString _uuid )
{
    return NULL;
}



bool Razordev::getMounted()
{
    return false;
}


QString Razordev::getuuid()
{
    return "";
}

QString Razordev::getvolume()
{
    return "";
}

bool Razordev::mount()
{
    return false;
}

bool Razordev::umount()
{
    return false;
}

QList< QString > Razordevman::listDevices()
{
    QList < QString> tmp;
    return tmp;
}

bool Razordevman::mount ( QString _uuid )
{
    return false;
}

bool Razordevman::umount ( QString _uuid )
{
    return false;
}




#endif
