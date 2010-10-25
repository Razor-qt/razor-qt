#ifndef RAZOR_LITE_H
#define RAZOR_LITE_H
#include "defs.h"
#include <razorqt/xfitman.h>
#include <razorqt/xdgmanager.h>
#include "razorgui.h"
#include <razorqt/readsettings.h>
#include "razorhandler.h"
#include "razorevent.h"
#include "razorpluginmanager.h"
/**
 * @file razor.h
 * @brief declares class Razor
 * @author Christopher "VdoP" Regali
 */

/**
 * @brief manages all of the stuff - Implemented as a singleton
 */
class Razor
{
public:
    ~Razor();
    void setupGui();
    void setupEvents(int& argc, char** argv);
    static Razor& getInstance();

    Xfitman* get_Xfitman()
    {
        return xman;
    }
    Xdgmanager* get_Xdgmanager()
    {
        return xdgmanager;
    }
    Readsettings* get_settings()
    {
        return settings;
    }
    Readsettings* get_looknfeel()
    {
        return looknfeel;
    }
    Razorhandler* get_handler()
    {
        return rhandler;
    }
    Razorgui* get_gui()
    {
        return razorgui;
    }
    Razorevent* get_events()
    {
        return razorevent;
    }
    Razorpluginmanager* get_plugins()
    {
        return pluginmanager;
    }
private:
    int argc;
    char** argv;
    Razor();
    Razor(Razor& cc);
    Razorhandler* rhandler;
    Readsettings* settings;
    Readsettings* looknfeel;
    Xfitman* xman;
    Xdgmanager* xdgmanager;
    Razorgui* razorgui;
    Razorevent* razorevent;
    Razorpluginmanager* pluginmanager;

};




#endif
