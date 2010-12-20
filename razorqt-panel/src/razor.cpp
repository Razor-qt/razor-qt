#ifndef RAZOR_LITE_CPP
#define RAZOR_LITE_CPP

#include "razor.h"
/**
 * @file razor.cpp
 * @author Christopher "VdoP" Regali
 */



/**
 * @brief overloaded for call without arguments
 */
Razor &Razor::getInstance()
{
    static Razor readinstance;
    return readinstance;
}

/**
 * @brief sets up the application class
 */
void Razor::setupEvents(int& argc, char** argv)
{

    //startup a QApp-inheritor to parse them
    razorevent = new RazorEvent(argc, argv);
}



/**
 * @brief the constructor - only setup non gui stuff here
 */
Razor::Razor()
{
    cfg = new ReadSettings("razor");
    m_theme = new ReadTheme(cfg->settings()->value("theme", "").toString());
    rhandler = new RazorHandler;
}


/**
 * @brief do the gui stuff here
 */
bool Razor::setupGui()
{
    xdgmanager = new XdgManager(cfg->settings()->value("icon_theme", "").toString());
    ReadTheme theme(cfg->settings()->value("theme", "").toString());
    razorevent->setStyleSheet(theme.qss());

    //make an xfitman and set the correct events for us to be parsed
    xman = new XfitMan;
    xman->setEventRoute();
    //ok gui setup
    razorgui = new RazorGUI;
    //now load the plugins
    pluginmanager = new RazorPluginManager;

    return true;
}

/**
 * @brief the destructor
 */
Razor::~Razor()
{
    delete razorgui;
    delete xman;
    delete xdgmanager;
    delete cfg;
}


#endif
