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
 
#include "mainwindow.h"

#include <QtCore/QSettings>
#include <QtGui/QProgressDialog>
#include <qtxdg/xdgdirs.h>
#include <qtxdg/xdgicon.h>
#include <razorqt/razorsettings.h>


MainWindow::MainWindow()
{
    setupUi(this);
    listWidget->setIconSize(QSize(64,64));
    
    m_settings = new RazorSettings("razor", this);
    m_cache = new RazorSettingsCache(m_settings);

//    qDebug() << "START" << XdgDirs::dataDirs();
    
    QDir d;
    QStringList dirs = XdgDirs::dataDirs();

    QProgressDialog progress(tr("Searching for icon themes"), "", 0, dirs.count(), this);
    progress.setCancelButton(0);
    progress.setMinimumDuration(0);
    progress.show();
    qDebug() << "AAAAA";
    qApp->processEvents();

    int i = 0;
    foreach(QString basedir, dirs)
    {
        progress.setValue(i);
        qApp->processEvents();
        ++i;

//        // TODO/FIXME: think about this "optimization"
        if (basedir == "/usr/share")
            basedir = "/usr/share/icons";

//        qDebug() << "BASEDIR" << basedir;
        QDirIterator it(basedir, QStringList() << "index.theme", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            QFileInfo fi(it.next());
            qDebug() << fi.filePath();
            QSettings check(fi.filePath(), QSettings::IniFormat);
            if (check.childGroups().contains("Icon Theme"))
            {
                if (d.exists(fi.canonicalPath() + "/" + "cursors"))
                {
//                    qDebug() << "    mouse theme. Skipped";
                }
                else
                {
//                    qDebug() << "  ADD" << fi.dir().dirName();
                    XdgIcon::setThemeName(fi.dir().dirName());
                    QString sample = check.value("Icon Theme/Example", "application-exit").toString();
                    new QListWidgetItem(XdgIcon::fromTheme(sample), fi.dir().dirName(), listWidget);
                }
            }
        }
    }
    progress.hide();

    restoreSettings();

    connect(buttons, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(dialogButtonsAction(QAbstractButton*)));
    connect(listWidget, SIGNAL(currentTextChanged(const QString &)),
            this, SLOT(listWidget_currentTextChanged(const QString &)));
}

MainWindow::~MainWindow()
{
    delete m_cache;
}

void MainWindow::listWidget_currentTextChanged(const QString &text)
{
//    qDebug() << "listWidget_currentTextChanged" << text;
    if (text.isNull())
        return;

    m_settings->setValue("icon_theme", text);
}

void MainWindow::restoreSettings()
{
    QString theme = m_settings->value("icon_theme", "oxygen").toString();
//    qDebug() << "SETTINGS" << theme;
    foreach (QListWidgetItem *item, listWidget->findItems(theme, Qt::MatchFixedString))
    {
//        qDebug() << "FOUND" << item;
        listWidget->setCurrentItem(item);
        break;
    }
}

void MainWindow::dialogButtonsAction(QAbstractButton *btn)
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

