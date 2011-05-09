#ifndef DESKTOPPLUGIN_H
#define DESKTOPPLUGIN_H

#include <razorqt/readsettings.h>


class DesktopPlugin
{
public:
    DesktopPlugin(const QString & configId, ReadSettings * config, ReadTheme * theme)
        : m_config(config),
          m_theme(theme)
    {
    };

    virtual QString info() { return QObject::tr("No info available"); };

protected:
    ReadSettings * m_config;
    ReadTheme * m_theme;
};


/*! Prototype for plugin's init() function
 */
typedef DesktopPlugin* (*PluginInitFunction)(const QString & configId, ReadSettings * config, ReadTheme * theme);


/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin header file.
 */
#define EXPORT_RAZOR_DESKTOP_PLUGIN_H \
    extern "C" DesktopPlugin* init(const QString & configId, ReadSettings * config, ReadTheme * theme);

/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin source file.
 */
#define EXPORT_RAZOR_DESKTOP_PLUGIN_CPP(PLUGINCLASS)        \
    DesktopPlugin* init(const QString & configId, ReadSettings * config, ReadTheme * theme) \
    {                                                       \
        return new PLUGINCLASS(configId, config, theme);      \
    }


#endif
