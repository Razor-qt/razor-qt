/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef DESKTOPWIDGETPLUGIN_H
#define DESKTOPWIDGETPLUGIN_H


#include <razorqt/razorsettings.h>


class DesktopWidgetPlugin
{
public:
    DesktopWidgetPlugin(QGraphicsScene * scene, const QString & configId, RazorSettings * config)
        : m_config(config),
          m_configId(configId)
    {
    };

    virtual QString info() { return QObject::tr("No info available"); };
    virtual QString instanceInfo() = 0;
    
    virtual void setSizeAndPosition(const QPointF & position, const QSizeF & size) = 0;
    virtual void save() = 0;
    virtual void configure() = 0;
    
    virtual bool blockGlobalMenu() = 0;
    
    QString configId() { return m_configId; }

    //! \todo TODO/FIXME: maybe a candidate for RazorSettings...
    void removeConfig()
    {
        m_config->beginGroup(m_configId);
        m_config->remove("");
        m_config->endGroup();
    }

protected:
    RazorSettings * m_config;
    QString m_configId;
};


/*! Prototype for plugin's init() function
 */
typedef DesktopWidgetPlugin* (*DesktopWidgetInitFunction)(QGraphicsScene * scene, const QString & configId, RazorSettings * config);


/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin header file.
 */
#define EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_H \
    extern "C" DesktopWidgetPlugin* init(QGraphicsScene * scene, const QString & configId, RazorSettings * config);

/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin source file.
 */
#define EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_CPP(PLUGINCLASS)        \
    DesktopWidgetPlugin* init(QGraphicsScene * scene, const QString & configId, RazorSettings * config)              \
    {                                                       \
        return new PLUGINCLASS(scene, configId, config);      \
    }


#endif
