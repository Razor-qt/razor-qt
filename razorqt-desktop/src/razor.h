#ifndef RAZOR_H
#define RAZOR_H
#include <razorqt/readsettings.h>
#include <razorqt/xfitman.h>
#include <razorqt/xdgenv.h>
#include <razorqt/xdgiconthememanager.h>

class Razor
{
public:
    static Razor& getInstance();
    ~Razor();
    ReadSettings* razorsettings()
    {
        return razorSettings;
    }
    ReadSettings* themesettings()
    {
        return themeSettings;
    }
    XfitMan* getxfitman()
    {
        return xfitman;
    }
    XdgEnv* getxdgenv()
    {
        return xdgenv;
    }
    XdgIconThemeManager* geticontheme()
    {
        return xdgicon;
    }

private:
    Razor();
    Razor(Razor& cc);
    ReadSettings* razorSettings;
    ReadSettings* themeSettings;
    XfitMan*	    xfitman;
    XdgEnv*	    xdgenv;
    XdgIconThemeManager* xdgicon;
};

#endif

