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

#ifndef NOTIFICATIONWIDGETS_H
#define NOTIFICATIONWIDGETS_H

#include <QtCore/QHash>
#include <QtGui/QWidget>
#include <QtGui/QAbstractButton>

class QComboBox;


/*! A helper widgets for actions handling.
 * See specification for information what actions are.
 *
 * If there are only few actions the layout with buttons is used.
 * If there are more actions the combo box with confirm button is created.
 */
class NotificationActionsWidget : public QWidget
{
    Q_OBJECT

public:
    /*! Create new widget.
     * \param actions a list of actions in form: (key1, display1, key2, display2, ..., keyN, displayN)
     */
    NotificationActionsWidget(const QStringList& actions, QWidget *parent);

signals:
    /*! User clicks/chose an action
     * \param actionKey a key of selected action
     */
    void actionTriggered(const QString &actionKey);

private:
    QComboBox *m_comboBox;
    QHash<QString,QString> m_actionMap;

private slots:
    void actionComboBoxActivated();
    void actionButtonActivated(QAbstractButton* button);
};

#endif
