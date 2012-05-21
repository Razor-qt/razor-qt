/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2012 Razor team
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

#include "defaultappspage.h"
#include "ui_defaultappspage.h"

#include "sessionconfigwindow.h"

DefaultApps::DefaultApps(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DefaultAppsPage)
{
    ui->setupUi(this);

    connect(ui->terminalButton, SIGNAL(clicked()), this, SLOT(terminalButton_clicked()));
    connect(ui->terminalComboBox, SIGNAL(editTextChanged(QString)),
            SLOT(terminalChanged(QString)));
    connect(ui->browserButton, SIGNAL(clicked()), this, SLOT(browserButton_clicked()));
    connect(ui->browserComboBox, SIGNAL(editTextChanged(QString)),
            SLOT(browserChanged(QString)));
}

DefaultApps::~DefaultApps()
{
    delete ui;
}

void DefaultApps::updateEnvVar(const QString& var, const QString& val)
{
    if (var == "BROWSER")
    {
        QStringList knownBrowsers;
        knownBrowsers << "firefox" << "qupzilla" << "arora" << "konqueror" << "opera";
        SessionConfigWindow::handleCfgComboBox(ui->browserComboBox, knownBrowsers, val);
    }
    else if (var == "TERM")
    {
        QStringList knownTerms;
        knownTerms << "qterminal" << "xterm" << "konsole" << "uterm";
        SessionConfigWindow::handleCfgComboBox(ui->terminalComboBox, knownTerms, val);
    }
}

void DefaultApps::terminalButton_clicked()
{
    SessionConfigWindow::updateCfgComboBox(ui->terminalComboBox, tr("Select a terminal emulator"));
}

void DefaultApps::browserButton_clicked()
{
    SessionConfigWindow::updateCfgComboBox(ui->browserComboBox, tr("Select a web browser"));
}

void DefaultApps::terminalChanged(const QString& val)
{
    emit defaultAppChanged("TERM", val);
}

void DefaultApps::browserChanged(const QString& val)
{
    emit defaultAppChanged("BROWSER", val);
}
