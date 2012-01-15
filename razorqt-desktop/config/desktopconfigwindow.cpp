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

#include <QtCore/QProcess>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>

#include <razorqt/razorsettings.h>
#include <qtxdg/xdgicon.h>

#include "desktopconfigwindow.h"



DesktopConfigWindow::DesktopConfigWindow()
    : QMainWindow(),
      m_restart(false)
{
    setupUi(this);

    // pages
    new QListWidgetItem(XdgIcon::fromTheme("preferences-desktop"), tr("Basic Settings"), listWidget);
    new QListWidgetItem(XdgIcon::fromTheme("show-menu"), tr("Menu Configuration"), listWidget);
    new QListWidgetItem(XdgIcon::fromTheme("preferences-desktop-personal"), tr("WM Native Desktop"), listWidget);
    listWidget->setCurrentRow(0);
    
    desktopTypeComboBox->addItem("Razor Desktop", "razor");
    desktopTypeComboBox->addItem("Window Manager Native", "wm_native");
    
    m_settings = new RazorSettings("desktop", this);
    m_cache = new RazorSettingsCache(m_settings);
    restoreSettings();
    
    connect(desktopTypeComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(desktopTypeComboBox_currentIndexChanged(int)));
    // UI stuff
    connect(chooseMenuFilePB, SIGNAL(clicked()), this, SLOT(chooseMenuFile()));
    //
    connect(nativeWallpaperButton, SIGNAL(clicked()), this, SLOT(nativeWallpaperButton_clicked()));
    //
    // notify it needs restart
    connect(singleclickButton, SIGNAL(clicked()), this, SLOT(setRestart()));
    connect(doubleclickButton, SIGNAL(clicked()), this, SLOT(setRestart()));
    connect(nativeIconsCheckBox, SIGNAL(clicked()), this, SLOT(setRestart()));
    connect(nativeWallpaperEdit, SIGNAL(textChanged(const QString&)), this, SLOT(setRestart()));
    connect(wheelDesktopCheckBox, SIGNAL(toggled(bool)), this, SLOT(setRestart()));
    connect(useCommonWallpaperCheckBox, SIGNAL(clicked()), this, SLOT(setRestart()));
    //
    connect(buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));
}

DesktopConfigWindow::~DesktopConfigWindow()
{
    delete m_cache;
}

void DesktopConfigWindow::restoreSettings()
{
    // basic settings ****************************************************

    QString desktopType = m_settings->value("desktop", "razor").toString();

    int ix = desktopTypeComboBox->findData(desktopType);
    if (ix == -1)
        ix = 0;
    desktopTypeComboBox->setCurrentIndex(ix);
    desktopTypeComboBox_currentIndexChanged(ix);
    
    QString clickType = m_settings->value("icon-launch", "singleclick").toString();
    if (clickType == "singleclick")
        singleclickButton->setChecked(true);
    else
        doubleclickButton->setChecked(true);
    
    // razor
    m_settings->beginGroup("razor");
    menuFilePathLE->setText(m_settings->value("menu_file").toString());
    wheelDesktopCheckBox->setChecked(m_settings->value("mouse_wheel_desktop_switch", false).toBool());
    useCommonWallpaperCheckBox->setChecked(m_settings->value("use_common_wallpaper", true).toBool());
    m_settings->endGroup();
    
    // wm_native
    m_settings->beginGroup("wm_native");
    QString wmWallpaper = m_settings->value("wallpaper").toString();
    nativeWallpaperEdit->setText(wmWallpaper);
    bool wmIcons = m_settings->value("icons", true).toBool();
    nativeIconsCheckBox->setChecked(wmIcons);
    m_settings->endGroup();
    
    m_restart = false;
}

void DesktopConfigWindow::closeEvent(QCloseEvent * event)
{
    m_settings->setValue("desktop", desktopTypeComboBox->itemData(desktopTypeComboBox->currentIndex()).toString());
    m_settings->setValue("icon-launch", singleclickButton->isChecked() ? "singleclick" : "doubleclick");

    m_settings->beginGroup("razor");    
    if (!menuFilePathLE->text().isEmpty())
    {
        m_settings->setValue("menu_file", menuFilePathLE->text());
    }
    m_settings->setValue("mouse_wheel_desktop_switch", wheelDesktopCheckBox->isChecked());
    m_settings->setValue("use_common_wallpaper", useCommonWallpaperCheckBox->isChecked());
    m_settings->endGroup();
    
    m_settings->beginGroup("wm_native");
    m_settings->setValue("wallpaper", nativeWallpaperEdit->text());
    m_settings->setValue("icons", nativeIconsCheckBox->isChecked());
    m_settings->endGroup();
    
    if (!m_restart)
        return;

    if (QMessageBox::question(this, tr("Restart Desktop Module?"),
                            tr("Razor Desktop module needs to be restarted to apply changes. Restart?"),
                            QMessageBox::Yes, QMessageBox::No)
        == QMessageBox::Yes)
    {
        QProcess::execute("killall razor-desktop");
    }
}

void DesktopConfigWindow::desktopTypeComboBox_currentIndexChanged(int ix)
{
    QString t = desktopTypeComboBox->itemData(ix).toString();
    if (t == "razor")
    {
        desktopTypeLabels->setText(tr("<b>Razor Desktop</b><br>"
                                      "Desktop is fully controlled by Razor. <b>Configuration is available in the desktop context menu.</b>"
                                     )
                                  );
    }
    else
    {
        desktopTypeLabels->setText(tr("<b>Window Manager Native Desktop</b><br>"
                                      "Desktop is handled by used window manager (including WM's own menus etc.). Only few things can be configured here."
                                     )
                                  );
    }
    
    m_restart = true;
}

void DesktopConfigWindow::chooseMenuFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Choose menu file"), "~", tr("Menu files (*.menu)"));
    if (!path.isEmpty())
    {
        menuFilePathLE->setText(path);
        m_restart = true;
    }
}

void DesktopConfigWindow::nativeWallpaperButton_clicked()
{
    QString fname = QFileDialog::getOpenFileName(this, tr("Select Wallpaper Image"),
                                                 QDir::homePath(), tr("Images (*.png *.xpm *.jpg)"));
    if (fname.isNull())
        return;
    
    nativeWallpaperEdit->setText(fname);
}

void DesktopConfigWindow::setRestart()
{
    m_restart = true;
}

void DesktopConfigWindow::dialogButtonsAction(QAbstractButton *btn)
{
    if (buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        m_cache->loadToSettings();
        restoreSettings();
    }
    else
    {
        close();
    }
}
