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
    razorSettings = new ReadSettings("razor.conf");
    themeSettings = new ReadSettings(razorSettings->getString("style_theme"));
    xfitman = new XfitMan();
    xdgenv = new XdgEnv();
    xdgicon = new XdgIconThemeManager(razorSettings->getString("icon_theme"),xdgenv);
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
