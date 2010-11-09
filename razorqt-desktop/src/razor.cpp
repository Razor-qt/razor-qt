#ifndef RAZOR_CPP
#define RAZOR_CPP
#include "razor.h"


Razor &Razor::getInstance()
{
    static Razor readinstance;
    return readinstance;
}

Razor::Razor()
{
    razorSettings = new Readsettings("razor.conf");
    themeSettings = new Readsettings(razorSettings->getString("style_theme"));
    xfitman = new Xfitman();
    xdgenv = new Xdgenv();
    xdgicon = new Xdgiconthememanager(razorSettings->getString("icon_theme"),xdgenv);
}


Razor::Razor(Razor& cc)
{
}



Razor::~Razor()
{
    delete razorSettings;
    delete themeSettings;
    delete xdgicon;
    delete xdgenv;
    delete xfitman;
}



#endif
