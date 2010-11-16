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
    settings = new ReadSettings("razor.conf");
    rhandler = new RazorHandler;
}


/**
 * @brief do the gui stuff here
 */
void Razor::setupGui()
{

    looknfeel = new ReadSettings(settings->getString("style_theme"));
    xdgmanager = new XdgManager(settings->getString("icon_theme"));
    //select stylesheet from theme
    QFile stylesheet(looknfeel->getPath() + looknfeel->getString("panel_stylesheet"));
    QString sheet="";
    if (stylesheet.exists())
    {
        stylesheet.open(QIODevice::ReadOnly | QIODevice::Text);
        sheet=stylesheet.readAll();
    }
    qDebug() << "loading sheet: " << stylesheet.fileName() << " with content: " << sheet;
    razorevent->setStyleSheet(sheet);
    //make an xfitman and set the correct events for us to be parsed
    xman = new XfitMan;
    xman->setEventRoute();
    //ok gui setup
    razorgui = new RazorGUI;
    //now load the plugins
    pluginmanager = new RazorPluginManager;
}

/**
 * @brief the destructor
 */
Razor::~Razor()
{
    delete razorgui;
    delete xman;
    delete xdgmanager;
    delete looknfeel;
    delete settings;
}


#endif
