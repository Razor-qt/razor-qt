/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
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

#ifndef NOTIFICATIONAREA_H
#define NOTIFICATIONAREA_H

#include <QtGui/QScrollArea>
#include "notificationlayout.h"


/*! Top level widget. Scroll area is used to ensure access
 * of all \c Notification instances (scrollable with mouse).
 */
class NotificationArea : public QScrollArea
{
    Q_OBJECT
public:

    explicit NotificationArea(QWidget *parent = 0);

    /*! An access to \c NotificationLayout to connect signals and slots in \c Notifyd
     */
    NotificationLayout* layout() { return m_layout; }

    /*! Set new settings value from \c Notifyd. There are only one settings
     * used - in \c Notifyd.
     * \param placement a string name for notification location "top-level" etc.
     * \param width set with of notifications
     * \param spacing a spacing in the \NotificationLayout
     */
    void setSettings(const QString &placement, int width, int spacing);

private:
    NotificationLayout *m_layout;

    QString m_placement;
    int m_spacing;

private slots:
    /*! Recalculate widget size and visibility. Slot is called from \c Notificationlayout
     * on demand (notification appear or is closed).
     */
    void setHeight(int contentHeight);
};

#endif // NOTIFICATIONAREA_H
