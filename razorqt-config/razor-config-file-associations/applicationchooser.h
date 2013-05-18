/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Christian Surlykke
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

#ifndef _APPLICATIONCHOOSER_H
#define	_APPLICATIONCHOOSER_H

#include "ui_applicationchooser.h"
#include "libraries/qtxdg/xdgdesktopfile.h"

class XdgMimeInfo;
class QSettings;

class ApplicationChooser : public QDialog
{
    Q_OBJECT
public:
    ApplicationChooser(XdgMimeInfo* mimeInfo, bool showUseAlwaysCheckBox = false);
    virtual ~ApplicationChooser();

    void fillApplicationListWidget();

    XdgDesktopFile* DefaultApplication() const { return m_CurrentDefaultApplication; }

private slots:
    void selectionChanged();

private:
    void addApplicationsToApplicationListWidget(QTreeWidgetItem* parent, 
                                                QList<XdgDesktopFile*> applications, 
                                                QSet<XdgDesktopFile*> & alreadyAdded);
    XdgMimeInfo* m_MimeInfo;
    Ui::ApplicationChooser widget;
    XdgDesktopFile* m_CurrentDefaultApplication;
};

#endif	/* _APPLICATIONCHOOSER_H */
