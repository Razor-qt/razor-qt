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

#include "basicsettings.h"
#include "ui_basicsettings.h"

#include "../razor-session/src/windowmanager.h"
#include "sessionconfigwindow.h"

BasicSettings::BasicSettings(RazorSettings *settings, QWidget *parent) :
    QWidget(parent),
    m_settings(settings),
    m_moduleModel(new ModuleModel()),
    ui(new Ui::BasicSettings)
{
    ui->setupUi(this);
    connect(ui->findWmButton, SIGNAL(clicked()), this, SLOT(findWmButton_clicked()));
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startButton_clicked()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopButton_clicked()));
    connect(ui->wmComboBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(setRestart()));
    connect(ui->wmComboBox, SIGNAL(editTextChanged(const QString&)), SIGNAL(needRestart()));
    restoreSettings();

    ui->moduleView->setModel(m_moduleModel);
    ui->moduleView->header()->setResizeMode(0, QHeaderView::Stretch);
    ui->moduleView->header()->setResizeMode(1, QHeaderView::ResizeToContents);
}

BasicSettings::~BasicSettings()
{
    delete ui;
}

void BasicSettings::restoreSettings()
{
    QStringList knownWMs;
    foreach (WindowManager wm, getWindowManagerList(true))
    {
        knownWMs << wm.command;
    }

    QString wm = m_settings->value("windowmanager", "openbox").toString();
    SessionConfigWindow::handleCfgComboBox(ui->wmComboBox, knownWMs, wm);
    m_moduleModel->reset();
}

void BasicSettings::save()
{
    m_settings->setValue("windowmanager", ui->wmComboBox->currentText());
    m_moduleModel->writeChanges();
}

void BasicSettings::findWmButton_clicked()
{
    SessionConfigWindow::updateCfgComboBox(ui->wmComboBox, tr("Select a window manager"));
    emit needRestart();
}

void BasicSettings::startButton_clicked()
{
    m_moduleModel->toggleModule(ui->moduleView->selectionModel()->currentIndex(), true);
}

void BasicSettings::stopButton_clicked()
{
    m_moduleModel->toggleModule(ui->moduleView->selectionModel()->currentIndex(), false);
}
