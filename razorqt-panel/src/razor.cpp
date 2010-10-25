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
    razorevent = new Razorevent(argc, argv);
}



/**
 * @brief the constructor - only setup non gui stuff here
 */
Razor::Razor()
{
    settings = new Readsettings("razor.conf");
    looknfeel = new Readsettings(settings->getValue("style_theme"));
    xdgmanager = new Xdgmanager(settings->getValue("icon_theme"));
    rhandler = new Razorhandler;
}


/**
 * @brief do the gui stuff here
 */
void Razor::setupGui()
{
    //select stylesheet from theme
    QFile stylesheet(looknfeel->getPath() + looknfeel->getValue("panel_stylesheet"));
    QString sheet="";
    if (stylesheet.exists())
    {
      stylesheet.open(QIODevice::ReadOnly | QIODevice::Text);
      sheet=stylesheet.readAll();
    }
    qDebug() << "loading sheet: " << stylesheet.fileName() << " with content: " << sheet;
    razorevent->setStyleSheet(sheet);
    //make an xfitman and set the correct events for us to be parsed
    xman = new Xfitman;
    xman->setEventRoute();
    //ok gui setup
    razorgui = new Razorgui;
    //now load the plugins
    pluginmanager = new Razorpluginmanager;
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
