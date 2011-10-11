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


#ifndef RAZORQUICKLAUNCH_H
#define RAZORQUICKLAUNCH_H

#include "../panel/razorpanelplugin.h"
#include "../panel/razorpanel.h"
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtGui/QAction>

class XdgDesktopFile;
class QAction;
class QDragEnterEvent;


/*! \brief Loader for "quick launcher" icons in the panel.
\author Petr Vanek <petr@scribus.info>
*/
class RazorQuickLaunch : public RazorPanelPlugin
{
    Q_OBJECT

public:
    RazorQuickLaunch(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
    ~RazorQuickLaunch();

    virtual void iconThemeChanged();

private:
    QMap<QString,XdgDesktopFile*> m_xdgFiles;

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private slots:
    //! run command in the action
    void execAction(QAction*);
    void addButton(QAction* action);
};

/*! \brief Special action representation for RazorQuickLaunch plugin.
It supports XDG desktop files or "legacy" launching of specified apps.
All process management is handled internally.
\author Petr Vanek <petr@scribus.info>
*/
class RazorQuickLaunchAction : public QAction
{
    Q_OBJECT

public:
    /*! Constructor for "legacy" launchers.
        \warning The XDG way is preferred this is only for older or non-standard apps
        \param name a name to display in tooltip
        \param exec a executable with path
        \param icon a valid QIcon
     */
    RazorQuickLaunchAction(const QString & name,
                           const QString & exec,
                           const QString & iconName,
                           QWidget * parent);
    /*! Constructor for XDG desktop handlers.
     */
    RazorQuickLaunchAction(const XdgDesktopFile * xdg, QWidget * parent);
    /*! Constructor for regular files 
     */
    RazorQuickLaunchAction(const QString & fileName, QWidget * parent);

    //! Returns true if the action is valid (contains all required properties).
    bool isValid() { return m_valid; }

    void updateIcon();
    QString iconName() const { return mIconName; }
public slots:
    void execAction();

private:
    enum ActionType { ActionLegacy, ActionXdg, ActionFile };
    ActionType m_type;
    QString m_data;
    bool m_valid;
    QString mIconName;
};

EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
