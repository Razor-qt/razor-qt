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

#ifndef DESKTOPCONFIGWINDOW_H
#define DESKTOPCONFIGWINDOW_H

#include "ui_desktopconfigwindow.h"

class RazorSettings;
class RazorSettingsCache;
class QStringListModel;


class DesktopConfigWindow : public QMainWindow, public Ui::DesktopConfigWindow
{
    Q_OBJECT
    
public:
    DesktopConfigWindow();
    ~DesktopConfigWindow();
    
private:
    RazorSettings *m_settings;
    RazorSettingsCache *m_cache;

    // display restart warning
    bool m_restart;

    void closeEvent(QCloseEvent * event);
    void restoreSettings();

private slots:
    void setRestart();

    void desktopTypeComboBox_currentIndexChanged(int ix);
    void chooseMenuFile();
    void nativeWallpaperButton_clicked();

    void dialogButtonsAction(QAbstractButton *btn);
};

#endif
