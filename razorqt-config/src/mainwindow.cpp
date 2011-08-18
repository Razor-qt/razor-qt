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

#include <QtCore/QDirIterator>

#include "mainwindow.h"
#include <QtDebug>

#include <qtxdg/xdgdesktopfile.h>
#include <qtxdg/xdgicon.h>
#include <razorqt/razoraboutdlg.h>


namespace RazorConfig {

class ConfigItem : public QListWidgetItem
{
    XdgDesktopFile *m_xdg;

public:
    ConfigItem(XdgDesktopFile *xdg, QListWidget *parent)
        : QListWidgetItem(parent),
          m_xdg(xdg)
    {
        setIcon(xdg->icon(XdgIcon::defaultApplicationIcon()));
        setText(xdg->comment().isEmpty() ? xdg->name() : xdg->comment());
    }

    void start()
    {
        m_xdg->startDetached();
    }
};

} // namespace RazorConfig


RazorConfig::MainWindow::MainWindow() : QMainWindow()
{
    setupUi(this);
    
    qDebug() << "Reading desktop files from dir:" << RAZOR_CONFIG_MODULES_DIR;
    
    QDirIterator it(RAZOR_CONFIG_MODULES_DIR, QStringList() << "*.desktop");
    int ix = 0;
    QString name;

    while (it.hasNext()) {
        name = it.next();
        XdgDesktopFile *xdg = new XdgDesktopFile(name, this);
        if (!xdg->isValid())
        {
            qDebug() << "INVALID DESKTOP FILE:" << name;
            delete xdg;
            continue;
        }
        
        new ConfigItem(xdg, listWidget);

        ++ix;
    }
    
    if (listWidget->count() == 0)
        statusBar()->showMessage(tr("No config modules found in: ") + RAZOR_CONFIG_MODULES_DIR);
    
    connect(action_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(action_About, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(listWidget_itemDoubleClicked(QListWidgetItem *)));
}


void RazorConfig::MainWindow::listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    // we know it's ConfigItem only.
    reinterpret_cast<ConfigItem*>(item)->start();
}

void RazorConfig::MainWindow::about()
{
    RazorAboutDLG d;
}

