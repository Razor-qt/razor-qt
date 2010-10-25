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
      Readsettings* razorsettings() {return razorSettings;}
      Readsettings* themesettings() {return themeSettings;}
      Xfitman* getxfitman() {return xfitman;}
      Xdgenv* getxdgenv() {return xdgenv;}
      Xdgiconthememanager* geticontheme() {return xdgicon;}
      
private:
      Razor();
      Razor(Razor& cc);
      Readsettings* razorSettings;
      Readsettings* themeSettings;
      Xfitman*	    xfitman;
      Xdgenv*	    xdgenv;
      Xdgiconthememanager* xdgicon;
};

#endif

