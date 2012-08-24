/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
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
 * version 2.1 of the License, or (at your option) any later version.
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


#include <QtGui/QGraphicsObject>
#include <razorqt/razorsettings.h>

class DesktopScene;


class DesktopWidgetPlugin : public QGraphicsObject
{
    Q_OBJECT
public:
    DesktopWidgetPlugin(DesktopScene *scene, const QString & configId, RazorSettings * config);

    static int ZVAL() { return 2; }

    virtual QString info() { return QObject::tr("No info available"); };
    virtual QString instanceInfo() = 0;

    // Use it to set m_boundingRect
    virtual void setSizeAndPosition(const QPointF & position, const QSizeF & size);
    virtual void save() = 0;
    virtual void configure() = 0;


    RazorSettings *config() { return m_config; }
    QString configId() { return m_configId; }

    virtual QRectF boundingRect() const;

    //! \todo TODO/FIXME: maybe a candidate for RazorSettings...
    void removeConfig()
    {
        m_config->beginGroup(m_configId);
        m_config->remove("");
        m_config->endGroup();
    }

    void setEditable(bool editable);
    bool editable() { return m_editable; }

protected:
    RazorSettings * m_config;
    QString m_configId;
    QRectF m_boundingRect;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

private:
    enum ChangeMode {
        C_TL, C_T, C_TR,
        C_L,  C_C, C_R,
        C_BL, C_B, C_BR
    };

    ChangeMode m_mode;
    bool m_editable;
    bool m_highlight;

    QTimer * m_timer;

    QCursor getCursorByPos(const QPointF & position);

private slots:
    void setCursorByTimer();

};


/*! Prototype for plugin's init() function
 */
typedef DesktopWidgetPlugin* (*DesktopWidgetInitFunction)(DesktopScene *scene, const QString & configId, RazorSettings * config);


/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin header file.
 */
#define EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_H \
    extern "C" DesktopWidgetPlugin* init(DesktopScene *scene, const QString & configId, RazorSettings * config);

/*! Helper macro for define RazorPanelPlugin.
    Place this macro in your plugin source file.
 */
#define EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_CPP(PLUGINCLASS)        \
    DesktopWidgetPlugin* init(DesktopScene *scene, const QString & configId, RazorSettings * config)              \
    {                                                       \
        return new PLUGINCLASS(scene, configId, config);      \
    }


#endif
