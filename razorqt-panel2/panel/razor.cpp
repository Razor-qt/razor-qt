#ifndef RAZOR_LITE_CPP
#define RAZOR_LITE_CPP

#include "razor.h"
#include <razorqt/xfitman.h>

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
//    razorevent = new RazorEvent(argc, argv);
}



/**
 * @brief the constructor - only setup non gui stuff here
 */
Razor::Razor()
{
    cfg = new ReadSettings("razor");
    m_theme = new ReadTheme(cfg->settings()->value("theme", "").toString());
//    rhandler = new RazorHandler;
}


/**
 * @brief do the gui stuff here
 */
bool Razor::setupGui()
{
    ReadTheme theme(cfg->settings()->value("theme", "").toString());
//    razorevent->setStyleSheet(theme.qss());

    //make an xfitman and set the correct events for us to be parsed
    xman = new XfitMan;
    xman->setEventRoute();

    return true;
}

/**
 * @brief the destructor
 */
Razor::~Razor()
{
    delete xman;
    delete cfg;
}


#endif
