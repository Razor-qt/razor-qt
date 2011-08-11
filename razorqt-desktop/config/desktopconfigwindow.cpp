/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
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

#include <QtCore/QProcess>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>

#include <razorqt/razorsettings.h>
#include <qtxdg/xdgicon.h>

#include "desktopconfigwindow.h"



DesktopConfigWindow::DesktopConfigWindow()
    : QMainWindow()
{
    setupUi(this);

    // pages
    new QListWidgetItem(XdgIcon::fromTheme("preferences-desktop"), tr("Basic Settings"), listWidget);
    new QListWidgetItem(XdgIcon::fromTheme("preferences-desktop-personal"), tr("WM Native Desktop"), listWidget);
    listWidget->setCurrentRow(0);
    
    m_settings = new RazorSettings("desktop", this);
    
    // basic settings ****************************************************

    QString desktopType = m_settings->value("desktop", "razor").toString();
    desktopTypeComboBox->addItem("Razor Desktop", "razor");
    desktopTypeComboBox->addItem("Window Manager Native", "wm_native");
    int ix = desktopTypeComboBox->findData(desktopType);
    if (ix == -1)
        ix = 0;
    desktopTypeComboBox->setCurrentIndex(ix);
    desktopTypeComboBox_currentIndexChanged(ix);

    connect(desktopTypeComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(desktopTypeComboBox_currentIndexChanged(int)));
    
    QString clickType = m_settings->value("icon-launch", "singleclick").toString();
    if (clickType == "singleclick")
        singleclickButton->setChecked(true);
    else
        doubleclickButton->setChecked(true);
    
    // wm_native
    m_settings->beginGroup("wm_native");
    QString wmWallpaper = m_settings->value("wallpaper").toString();
    nativeWallpaperEdit->setText(wmWallpaper);
    bool wmIcons = m_settings->value("icons", true).toBool();
    nativeIconsCheckBox->setChecked(wmIcons);
    m_settings->endGroup();
    
    // UI stuff
    connect(action_Quit, SIGNAL(triggered()), this, SLOT(close()));
    connect(action_About, SIGNAL(triggered()), this, SLOT(about()));
    //
    connect(nativeWallpaperButton, SIGNAL(clicked()), this, SLOT(nativeWallpaperButton_clicked()));
}

void DesktopConfigWindow::closeEvent(QCloseEvent * event)
{
    m_settings->setValue("desktop", desktopTypeComboBox->itemData(desktopTypeComboBox->currentIndex()).toString());
    m_settings->setValue("icon-launch", singleclickButton->isChecked() ? "singleclick" : "doubleclick");
    
    m_settings->beginGroup("wm_native");
    m_settings->setValue("wallpaper", nativeWallpaperEdit->text());
    m_settings->setValue("icons", nativeIconsCheckBox->isChecked());
    m_settings->endGroup();
    
    if (QMessageBox::question(this, tr("Restart Desktop Module?"),
                            tr("Razor Desktop module needs to be restarted to apply changes. Restart?"),
                            QMessageBox::Yes, QMessageBox::No)
        == QMessageBox::Yes)
    {
        QProcess::execute("killall razor-desktop");
    }
}

void DesktopConfigWindow::about()
{
    QMessageBox::about(this,
                       tr("About Razor Desktop Config"),
                       tr("<h1>Razor Desktop Config</h1>"
                          "<p><a href=\"http://razor-qt.sf.net\">Part of the Razor Desktop</a></p>")
                      );
    
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
}

void DesktopConfigWindow::nativeWallpaperButton_clicked()
{
    QString fname = QFileDialog::getOpenFileName(this, tr("Select Wallpaper Image"),
                                                 QDir::homePath(), tr("Images (*.png *.xpm *.jpg)"));
    if (fname.isNull())
        return;
    
    nativeWallpaperEdit->setText(fname);
}
