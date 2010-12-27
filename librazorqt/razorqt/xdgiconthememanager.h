#ifndef XDGICONTHEMEMANAGER_H
#define XDGICONTHEMEMANAGER_H


/**
 * @file xdgiconthememanager.h
 * @brief Declares the class Xdgiconthememanager
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */

#include "defs.h"
#include "xdgiconthemefile.h"


/**
 * @brief shall manage the icon-themes and select the active one
 * @todo this class is a stub now.. needs some work
 */
class XdgIconThemeManager
{
public:
    XdgIconThemeManager(const QString & _filename);
    ~XdgIconThemeManager();
    //XdgIconThemeFile* get_activeTheme();
    QIcon getIconNG(const QString & _iconname);
    QString getIconpath(const QString & _iconfield,
                        const QString & _desktopfilepath,
                        const QString & _category="Applications");
    QMap<QString, QString> legacyMap;
private:
    QString theme;
    QString themePath;
    QString legacydir;
    QStringList foundThemes;
    XdgIconThemeFile* activeTheme;
    XdgIconThemeFile* fallbackTheme;

};

#endif
