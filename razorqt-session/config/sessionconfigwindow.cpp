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
 
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>

#include <razorqt/razorsettings.h>

#include "sessionconfigwindow.h"
#include "../src/windowmanager.h"
#include "basicsettings.h"
#include "autostartpage.h"
#include "defaultappspage.h"
#include "environmentpage.h"


SessionConfigWindow::SessionConfigWindow() :
      RazorConfigDialog(tr("Razor Session Configuration"), new RazorSettings("session"), 0)
{
    BasicSettings* basicSettings = new BasicSettings(mSettings, this);
    addPage(basicSettings, tr("Basic Settings"), "preferences-desktop-display-color");
    connect(basicSettings, SIGNAL(needRestart()), SLOT(setRestart()));
    connect(this, SIGNAL(reset()), basicSettings, SLOT(restoreSettings()));
    connect(this, SIGNAL(save()), basicSettings, SLOT(save()));

    DefaultApps* defaultApps = new DefaultApps(this);
    addPage(defaultApps, tr("Default Applications"), "preferences-desktop-filetype-association");

    AutoStartPage* autoStart = new AutoStartPage(this);
    addPage(autoStart, tr("Autostart"), "preferences-desktop-launch-feedback");
    connect(this, SIGNAL(reset()), autoStart, SLOT(restoreSettings()));
    connect(this, SIGNAL(save()), autoStart, SLOT(save()));

    EnvironmentPage* environmentPage = new EnvironmentPage(mSettings, this);
    addPage(environmentPage, tr("Environment (Advanced)"), "preferences-system-session-services");
    connect(environmentPage, SIGNAL(needRestart()), SLOT(setRestart()));
    connect(this, SIGNAL(reset()), environmentPage, SLOT(restoreSettings()));
    connect(this, SIGNAL(save()), environmentPage, SLOT(save()));

    // sync Default Apps and Environment
    connect(environmentPage, SIGNAL(envVarChanged(QString,QString)),
            defaultApps, SLOT(updateEnvVar(QString,QString)));
    connect(defaultApps, SIGNAL(defaultAppChanged(QString,QString)),
            environmentPage, SLOT(updateItem(QString,QString)));
    environmentPage->restoreSettings();
    connect(this, SIGNAL(reset()), SLOT(clearRestart()));
    m_restart = false;
}

SessionConfigWindow::~SessionConfigWindow()
{
    delete mSettings;
}

void SessionConfigWindow::closeEvent(QCloseEvent * event)
{
    RazorConfigDialog::closeEvent(event);
    if (m_restart) {
        QMessageBox::information(this, tr("Session Restart Required"),
                                tr("You need to restart desktop session (razor-session) "
                                    "to reload settings. Use logout from the main menu."
                                    )
                                );
    }
}

void SessionConfigWindow::handleCfgComboBox(QComboBox * cb,
                                            const QStringList &availableValues,
                                            const QString &value
                                           )
{
    if (value.isEmpty() || value == cb->lineEdit()->text())
        return;

    QStringList realValues;
    foreach (QString s, availableValues)
    {
        if (findProgram(s))
            realValues << s;
    }
    cb->clear();
    cb->addItems(realValues);

    int ix = cb->findText(value);
    if (ix == -1)
        cb->lineEdit()->setText(value);
    else
        cb->setCurrentIndex(ix);
}

void SessionConfigWindow::updateCfgComboBox(QComboBox * cb,
                                            const QString &prompt
                                           )
{
    QString fname = QFileDialog::getOpenFileName(cb, prompt, "/usr/bin/");
    if (fname.isEmpty())
        return;
    
    QFileInfo fi(fname);
    if (!fi.exists() || !fi.isExecutable())
        return;
    
    cb->lineEdit()->setText(fname);
}

void SessionConfigWindow::setRestart()
{
    m_restart = true;
}

void SessionConfigWindow::clearRestart()
{
    m_restart = false;
}
