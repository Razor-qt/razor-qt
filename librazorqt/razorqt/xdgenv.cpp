#ifndef XDGENV_CPP
#define XDGENV_CPP

#include "xdgenv.h"

//2010 Chris "VdoP" Regali
//Implements Xdgenv
/**
 * @file xdgenv.cpp
 * @brief Implements the class Xdgenv
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */


/**
 * @brief Constructor
 * Does automatically call updateEnv()
 */
XdgEnv::XdgEnv()
{
    qDebug() << "Xdgenv: initialising...";
    updateEnv();
}

/**
 * @brief the destructor
 * @todo Do This!
 */
XdgEnv::~XdgEnv()
{
}


/**
 * @brief returns a Value from the envMap
 * @param[in] _name the name of the Variable that is wanted
 * @return the value of the environmental variable
 */
QString XdgEnv::getEnv(QString _name)
{
    return envStore[_name];
}


/**
 * @brief the main function is this class
 * updates the variables via getenv, if they are not set, sets them to the standard-values according to XDG-Standard
 */
//updates the map of the env-variables
void XdgEnv::updateEnv()
{
    qDebug() << "Xdgenv: updating Environmentals";
    //try getting the correct XDG-ENVs
    //if not set, use the defaults from the XDG-Standart
    envStore["DATA_HOME"]=getenv("XDG_DATA_HOME");
    if (envStore["DATA_HOME"]=="")
        envStore["DATA_HOME"]=QDir::homePath() + "/.local/share/";

    envStore["CONFIG_HOME"] = getenv("XDG_CONFIG_HOME");
    if (envStore["CONFIG_HOME"] == "")
        envStore["CONFIG_HOME"] = QDir::homePath() + "/.config/";

    envStore["DATA_DIRS"] = getenv("XDG_DATA_DIRS");
    if (envStore["DATA_DIRS"]=="")
        envStore["DATA_DIRS"] = "/usr/local/share/:/usr/share/";

    envStore["CONFIG_DIRS"] = getenv("XDG_CONFIG_DIRS");
    if (envStore["CONFIG_DIRS"]=="")
        envStore["CONFIG_DIRS"] = "/usr/local/etc/xdg:/etc/xdg/";

    envStore["CACHE_HOME"] = getenv("XDG_CACHE_HOME");
    if (envStore["CACHE_HOME"]=="")
        envStore["CACHE_HOME"] = QDir::homePath() + "/.cache/";
}



//Tells the other-apps were here, bitches!

/**
 * @brief sets the XDG_MENU_PREFIX
 */
void XdgEnv::setEnv()
{
    qDebug() << "Xdgenv: setting menu prefix!";
    setenv("XDG_MENU_PREFIX","razor",1);
    envStore["MENU_PREFIX"]="razor";
}


/**
 * @brief puts some debug Information to qDebug()
 */
void XdgEnv::debugEnv()
{
    qDebug()<<"Xdgenv: Starting dump of stored variables";
    qDebug()<<"------------------------------------------";
    QMapIterator<QString, QString> i(envStore);
    while (i.hasNext())
    {
        i.next();
        qDebug() << i.key() << ": " << i.value();
    }
    qDebug()<<"------------------------------------------";
}



#endif
