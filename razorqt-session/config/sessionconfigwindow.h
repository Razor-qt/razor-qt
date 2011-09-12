/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef SESSIONCONFIGWINDOW_H
#define SESSIONCONFIGWINDOW_H

#include "ui_sessionconfigwindow.h"

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
    QStringListModel *m_autostartModel;
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
    void about();
    //
    void findWmButton_clicked();
    //
    void terminalButton_clicked();
    void browserButton_clicked();
    //
    void appAddButton_clicked();
    void appDeleteButton_clicked();
    //
    void envAddButton_clicked();
    void envDeleteButton_clicked();
    //
    void clearChanges();
    void setRestart();
};

#endif
