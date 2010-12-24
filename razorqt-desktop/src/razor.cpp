#ifndef RAZOR_CPP
#define RAZOR_CPP
#include "razor.h"
#include <razorqt/xdgenv.h>

Razor &Razor::getInstance()
{
    static Razor readinstance;
    return readinstance;
}

Razor::Razor()
{
    razorSettings = new ReadSettings("razor");
    themeSettings = new ReadTheme(razorSettings->settings()->value("theme", "").toString());
    xfitman = new XfitMan();
    QString iconTheme(razorSettings->settings()->value("icon_theme", "").toString());
    if (iconTheme.isEmpty())
        Q_ASSERT_X(0, "iconTheme is empty", "cannot continue without icon_theme in razor.conf!");
    xdgicon = new XdgIconThemeManager(iconTheme);
}


Razor::Razor(Razor& cc)
{
}



Razor::~Razor()
{
    delete razorSettings;
    delete themeSettings;
    delete xdgicon;
    delete xfitman;
}



#endif
