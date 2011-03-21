#ifndef DESKTOPPLUGIN_H
#define DESKTOPPLUGIN_H

#include <razorqt/readsettings.h>


class DesktopPlugin
{
public:
    DesktopPlugin(const QString & configId, ReadSettings * config)
        : m_config(config)
    {
    };

    virtual QString info() { return QObject::tr("No info available"); };

protected:
    ReadSettings * m_config;
};


/*! Prototype for plugin's init() function
 */
typedef DesktopPlugin* (*PluginInitFunction)(const QString & configId, ReadSettings * config);


/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin header file.
 */
#define EXPORT_RAZOR_DESKTOP_PLUGIN_H \
    extern "C" DesktopPlugin* init(const QString & configId, ReadSettings * config);

/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin source file.
 */
#define EXPORT_RAZOR_DESKTOP_PLUGIN_CPP(PLUGINCLASS)        \
    DesktopPlugin* init(const QString & configId, ReadSettings * config)              \
    {                                                       \
        return new PLUGINCLASS(configId, config);      \
    }


#endif
