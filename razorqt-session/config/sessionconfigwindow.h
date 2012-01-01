/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
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

#ifndef SESSIONCONFIGWINDOW_H
#define SESSIONCONFIGWINDOW_H

#include <QStringListModel>
#include <QtGui/QStandardItemModel>

#include <razorqt/xdgautostart.h>

#include "ui_sessionconfigwindow.h"
#include "autostartmodel.h"

class RazorSettings;
class RazorSettingsCache;
class QStringListModel;


class SessionConfigWindow : public QMainWindow, public Ui::SessionConfigWindow
{
    Q_OBJECT
    
public:
    SessionConfigWindow();
    ~SessionConfigWindow();
    
private:
    // RazorSettings are used as plain QSettings here because tha session cannot
    // reload cfg on the fly - it requires restart (ENV, autostart...)
    RazorSettings *m_settings;
    RazorSettingsCache *m_cache;
    AutoStartItemModel *mXdgAutoStartModel;
    // display restart warning
    bool m_restart;

    void handleCfgComboBox(QComboBox * cb,
                           const QStringList &availableValues,
                           const QString &value
                          );

    void updateCfgComboBox(QComboBox * cb, const QString &prompt);

    void closeEvent(QCloseEvent * event);

    void restoreSettings();

private slots:
    //
    void findWmButton_clicked();
    //
    void terminalButton_clicked();
    void browserButton_clicked();
    //
    void autoStartAddButton_clicked();
    void autoStartEditButton_clicked();
    void autoStartDeleteButton_clicked();
    //
    void envAddButton_clicked();
    void envDeleteButton_clicked();
    //
    void setRestart();

    void dialogButtonsAction(QAbstractButton *btn);
};

#endif
