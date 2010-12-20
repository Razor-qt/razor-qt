#ifndef XDGICONTHEMEMANAGER_CPP
#define XDGICONTHEMEMANAGER_CPP


/**
 * @file xdgiconthememanager.cpp
 * @brief Implements the class Xdgiconthemefile
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */

#include "xdgiconthememanager.h"

/**
 * @brief this uses the internal QIcon::fromTheme for getting the icons - the problem is: its qt 4.6 or later only!
 */

QIcon XdgIconThemeManager::getIconNG(const QString & _iconname)
{
    //if the names like bla.png - remove the .png
    if (_iconname.contains("."))
    {
        QStringList tmp = _iconname.split(".");
        tmp.removeLast();
        return getIconNG(tmp.join("."));
    }

    qDebug() << "Trying NG-Iconget: " << _iconname << theme;
    QStringList searchpath = QIcon::themeSearchPaths();
    if (!searchpath.contains(themePath))
    {
        searchpath.append(themePath);
        QIcon::setThemeSearchPaths(searchpath);

    }
    QIcon::setThemeName(theme);

    //first try to get it from theme
    if (QIcon::hasThemeIcon(_iconname))
    {
        qDebug() << "took from orig theme";
        return QIcon::fromTheme(_iconname);
    }
    else
    {
        QIcon::setThemeName("hicolor");
        if (QIcon::hasThemeIcon(_iconname))
        {

            qDebug() << "took from fallback theme";
            return QIcon::fromTheme(_iconname);
        }
        else
        {

            qDebug() << "took from legacy map";
            if (legacyMap.value(_iconname) != "" || _iconname == "system-run")
                return (QIcon) legacyMap.value(_iconname);
            else
                return getIconNG("system-run");
        }
    }
}



/**
 *@brief constructor with env and currently filename
 *@todo remove _filename parameter and make this thing search all paths for themes
 */
XdgIconThemeManager::XdgIconThemeManager(const QString & _filename, XdgEnv* _env)
{
    qDebug() << "Xdgiconthememanager: initialising...";
    iconEnv=_env;

    //prepare this for getIconNG

    QStringList tmp = _filename.split("/");
    tmp.removeLast();
    theme = tmp.last();
    tmp.removeLast();
    themePath = tmp.join("/");




    activeTheme=new XdgIconThemeFile(_filename);
    activeTheme->parseFile();
    activeTheme->searchDirs();
    //this is for fallback but needs also to be set via xdgenv!
    //actually hacked for bsd-compat
    QFile test("/usr/share/icons/");
    if (test.exists())
        fallbackTheme= new XdgIconThemeFile("/usr/share/icons/hicolor/index.theme");
    else
        fallbackTheme= new XdgIconThemeFile("/usr/local/share/icons/hicolor/index.theme");

    fallbackTheme->parseFile();
    fallbackTheme->searchDirs();

    //prepare a legacy-map


    QFile legtest("/usr/share/pixmaps");
    if (legtest.exists())
        legacydir="/usr/share/pixmaps";
    else
        legacydir="/usr/local/share/pixmaps";

    QDirIterator pix_iter(legacydir, QDirIterator::Subdirectories);
    while (pix_iter.hasNext())
    {
        pix_iter.next(); // move to child directory
        QFileInfo pix_file(pix_iter.fileInfo());
        if (pix_file.absoluteFilePath().endsWith(".png"))
            legacyMap[pix_file.baseName()] = pix_file.absoluteFilePath();
    }
    qDebug() << "xdgiconthememanager: having " << legacyMap.values().count() << " sets in legacymap of: " << legacydir;



    /* QDirIterator pix_iter_("/usr/share/icons", QDirIterator::Subdirectories);
     while (pix_iter_.hasNext())
     {
       pix_iter_.next(); // move to child directory
       QFileInfo pix_file(pix_iter_.fileInfo());
       if (pix_file.absoluteFilePath().endsWith(".png"))
         legacyMap[pix_file.baseName()] = pix_file.absoluteFilePath();
     }*/



}

/**
 * @brief destructor
 */
XdgIconThemeManager::~XdgIconThemeManager()
{
}

/**
 * @brief does the actual search for an icon by name or whatever!
 */

QString XdgIconThemeManager::getIconpath(const QString & _iconfield,
                                         const QString & _desktopfilepath,
                                         const QString & _category)
{
    qDebug() << _iconfield << "  " << _desktopfilepath;
    if (_iconfield=="")
        return "";
    //first try: maybe its already a full path!
    if (_iconfield.startsWith("/"))
        return _iconfield;
    //make a fileend purged copy of this shit
    QString iconpurged = _iconfield;
    if (iconpurged.endsWith(".png") || iconpurged.endsWith(".svg") || iconpurged.endsWith(".xpm"))
    {
        iconpurged.chop(4);


        //second try: its a file!
        QFile testlocal (_desktopfilepath + "/" + _iconfield);
        if (testlocal.exists())
            return testlocal.fileName();

        //third try: its a file in /usr/share/pixmaps
        QFile pixmaptest (legacydir+_iconfield);
        if (pixmaptest.exists())
            return pixmaptest.fileName();
    }

    //forth try get it from active theme!
    QString icon_file = activeTheme->searchIcon(iconpurged,_category);
    if (icon_file != "")
        return icon_file;

    //fifth try: use fallback theme!

    icon_file = fallbackTheme->searchIcon(iconpurged, _category);
    if (icon_file != "")
        return icon_file;

    // ok the standard officially ends here.. lets try our legacyMap!

    icon_file = legacyMap[iconpurged];
    if (icon_file != "")
        return icon_file;

    //now we really lost.. theres nothing we can actually do anymore but use a standart-pic!
    return "";

}



/**
 *@brief returns a pointer to the currently active theme
 */
XdgIconThemeFile* XdgIconThemeManager::get_activeTheme()
{
    return activeTheme;
}


#endif
