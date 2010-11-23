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
    bool setupGui();
    void setupEvents(int& argc, char** argv);
    static Razor& getInstance();

    XfitMan* get_Xfitman()
    {
        return xman;
    }
    XdgManager* get_Xdgmanager()
    {
        return xdgmanager;
    }
    ReadSettings* get_settings()
    {
        return settings;
    }
    ReadSettings* get_looknfeel()
    {
        return looknfeel;
    }
    RazorHandler* get_handler()
    {
        return rhandler;
    }
    RazorGUI* get_gui()
    {
        return razorgui;
    }
    RazorEvent* get_events()
    {
        return razorevent;
    }
    RazorPluginManager* get_plugins()
    {
        return pluginmanager;
    }
private:
    int argc;
    char** argv;
    Razor();
    Razor(Razor& cc);
    RazorHandler* rhandler;
    ReadSettings* settings;
    ReadSettings* looknfeel;
    XfitMan* xman;
    XdgManager* xdgmanager;
    RazorGUI* razorgui;
    RazorEvent* razorevent;
    RazorPluginManager* pluginmanager;

};




#endif
