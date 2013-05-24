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

#ifndef _MIMETYPEVIEWER_H
#define	_MIMETYPEVIEWER_H

#include <QDialog>
#include <QModelIndex>

#include "ui_mimetypeviewer.h"
#include "busyindicator.h"
#include "mimetypeitemmodel.h"

class QSettings;
class XdgMimeInfo;
class RazorSettingsCache;
class MimetypeFilterItemModel;

class MimetypeViewer : public QDialog {
    Q_OBJECT
public:
    MimetypeViewer(QWidget *parent = 0);
    virtual ~MimetypeViewer();

protected:
    void resizeEvent(QResizeEvent* event);

private slots:
    void initializeMimetypeTreeView();
    void currentMimetypeChanged();
    void autoExpandOnSearch();
    void chooseApplication();
    void dialogButtonBoxClicked(QAbstractButton *button);

private:
    void addSearchIcon();
    Ui::mimetypeviewer widget;
    MimetypeFilterItemModel m_MimetypeFilterItemModel;
    XdgMimeInfo* m_CurrentMime;
    QSettings* mDefaultsList;
    RazorSettingsCache *mSettingsCache;
    QFutureWatcher<void> *mFutureWatcher;
    BusyIndicator* mBusyIndicator;
};

#endif	/* _MIMETYPEVIEWER_H */
