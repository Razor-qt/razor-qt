#ifndef READSETTING_H
#define READSETTING_H

#include <QSettings>

/*! \brief Shared configuration handling.

Razor configuration is based on QSettings class.

This class, ReadSettings, opens one QSettings instance
based on INI file in the user home directory. ReadSettings
handles presence of the file too.

If there is no requested file found in the home directory the system-wide
installed file (see getSysPath()) is automatically copied into home
directory and then is it opened.

If there is no relevant system-wide file found the empty file is created.

ReadSettings user has to handle default values on their own. See
QSettings::value() default values.

\author Petr Vanek <petr@scribus.info>
*/
class ReadSettings : public QObject
{
    Q_OBJECT

public:
    /*! \brief Find and setup the configuration file.

    \param module a base name of the config file. It's a name without
            the extension. For example: if you want to open settings for
            panel call it as ReadSettings("panel"). Then access its
            settings over settings() method.
            The function will create all parent directories necessary to create
            the file.
            \note Existence of the QSettings instance is handled in this class.
                  Do not delete it on your own.
    \param parent It's no need to delete this class manually if it's set.
    */
    ReadSettings(const QString & module, QObject * parent = 0);
    ~ReadSettings() {};

    /*! \brief An access to QSettings object
    */
    QSettings* settings();

    const QString module() { return m_module; }

    /*! \brief Find location of the specified file or directory for Razor's cfg.
    The configuration is searched in more places:
      - user home dir (local config)
      - CMAKE_INSTALL_PREFIX location (the place set in the compilation time)
      - /usr/local/ prefix
      - /usr/ prefix
    It should handle all possibilities to get the requested file.
    \param fileName Name of the file including extension. Or name of the directory.
    */
    static QString getSysPath(const QString & fileName);

    /*! \brief Check existence of user config directory. Create it if it
    does not exist.
    */
    static bool checkConfigDir();

private:
    QSettings * m_settings;
    QString m_module;
};


/*! \brief Theme reader.
Theme contains QSS (a stylesheet description), icons, and background images.
All parts are optional.
ReadTheme search user home directory first, then the system-wide themes. See
ReadSettings::getSysPath() for more info.

\author Petr Vanek <petr@scribus.info>
*/
class ReadTheme : public QObject
{
    Q_OBJECT

public:
    /*! \brief Open, parse, and setup the GUI-theme.
    \param name a directory name of the theme
    \param parent No need to delete this object if it's set.
    */
    ReadTheme(const QString & name, QObject * parent = 0);
    /*! \brief Return QSS text (not-file, but real text).
    Paths in url() C/QSS functions are parsed to be real values for the theme,
    relative to full path
    */
    QString qss() { return m_qss; }

    /*! \brief Get icon path (not QIcon) by specified key. See theme docs
    for keys available.
    \param name a key for theme icon.
    */
    QString icon(const QString & name) { return m_icons[name]; }
    /*! \brief A full path to image used as a wallpaper
    */
    QString desktopBackground() { return m_desktopBackground; }
    /*! \brief A full path to image used as a splash screen
    */
    QString splashScreen() { return m_splashScreen; }
private:
    QString m_qss;
    QString m_qssPath;
    QHash<QString,QString> m_icons;
    QString m_desktopBackground;
    QString m_splashScreen;
};


#endif
