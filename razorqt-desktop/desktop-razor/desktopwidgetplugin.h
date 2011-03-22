#ifndef DESKTOPWIDGETPLUGIN_H
#define DESKTOPWIDGETPLUGIN_H


#include <razorqt/readsettings.h>


class DesktopWidgetPlugin
{
public:
    DesktopWidgetPlugin(QGraphicsScene * scene, const QString & configId, ReadSettings * config)
        : m_config(config),
          m_configId(configId)
    {
    };

    virtual QString info() { return QObject::tr("No info available"); };
    
    virtual void setSizeAndPosition(const QPointF & position, const QSizeF & size) = 0;

protected:
    ReadSettings * m_config;
    QString m_configId;
};


/*! Prototype for plugin's init() function
 */
typedef DesktopWidgetPlugin* (*DesktopWidgetInitFunction)(QGraphicsScene * scene, const QString & configId, ReadSettings * config);


/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin header file.
 */
#define EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_H \
    extern "C" DesktopWidgetPlugin* init(QGraphicsScene * scene, const QString & configId, ReadSettings * config);

/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin source file.
 */
#define EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_CPP(PLUGINCLASS)        \
    DesktopWidgetPlugin* init(QGraphicsScene * scene, const QString & configId, ReadSettings * config)              \
    {                                                       \
        return new PLUGINCLASS(scene, configId, config);      \
    }


#endif
