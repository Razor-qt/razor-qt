/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


#include "dialog.h"
#include "ui_dialog.h"
#include <razorqt/razorsettings.h>
#include <QtGui/QKeyEvent>
#include <QtGui/QToolButton>
#include <QDebug>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setStyleSheet(razorTheme->qss("razor-power/razor-power"));

    connect(ui->hibernateButton, SIGNAL(clicked()), this, SLOT(hibernate()));
    ui->hibernateButton->setVisible(mPower.canAction(RazorPower::PowerHibernate));

    connect(ui->logoutButton, SIGNAL(clicked()), this, SLOT(logout()));
    ui->logoutButton->setVisible(mPower.canAction(RazorPower::PowerLogout));

    connect(ui->rebootButton, SIGNAL(clicked()), this, SLOT(reboot()));
    ui->rebootButton->setVisible(mPower.canAction(RazorPower::PowerReboot));

    connect(ui->shutdownButton, SIGNAL(clicked()), this, SLOT(shutdown()));
    ui->shutdownButton->setVisible(mPower.canAction(RazorPower::PowerShutdown));

    connect(ui->suspendButton, SIGNAL(clicked()), this, SLOT(suspend()));
    ui->suspendButton->setVisible(mPower.canAction(RazorPower::PowerSuspend));

    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}


Dialog::~Dialog()
{
    delete ui;
}


void Dialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void Dialog::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        QToolButton *btn = qobject_cast<QToolButton*>(this->focusWidget());
        if (btn)
            btn->click();
        break;
    }

    QDialog::keyPressEvent(e);
}


void Dialog::hibernate()
{
    mPower.hibernate();
    close();
}


void Dialog::logout()
{
    mPower.logout();
    close();
}


void Dialog::reboot()
{
    mPower.reboot();
    close();
}

void Dialog::shutdown()
{
    mPower.shutdown();
    close();
}

void Dialog::suspend()
{
    mPower.suspend();
    close();
}
