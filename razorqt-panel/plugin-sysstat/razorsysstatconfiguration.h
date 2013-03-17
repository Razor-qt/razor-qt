/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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


#ifndef RAZORSYSSTATCONFIGURATION_H
#define RAZORSYSSTATCONFIGURATION_H

#include "razorqt/razorsettings.h"

#include <QtGui/QDialog>
#include <QtGui/QAbstractButton>
#include <QtCore/QMap>


namespace Ui {
    class RazorSysStatConfiguration;
}

namespace SysStat {
    class BaseStat;
}

class RazorSysStatColours;

class RazorSysStatConfiguration : public QDialog
{
    Q_OBJECT

public:
    explicit RazorSysStatConfiguration(QSettings *settings, QWidget *parent = 0);
    ~RazorSysStatConfiguration();

public slots:
    void saveSettings();

    void on_typeCOB_currentIndexChanged(int);
    void on_maximumHS_valueChanged(int);
    void on_customColoursB_clicked();
    void on_buttons_clicked(QAbstractButton *);

    void coloursChanged();

signals:
    void maximumNetSpeedChanged(QString);

private:
    Ui::RazorSysStatConfiguration *ui;
    QSettings *mSettings;
    RazorSettingsCache oldSettings;

    void loadSettings();

    SysStat::BaseStat *mStat;

    bool mLockSaving;

    RazorSysStatColours *mColoursDialog;
};

#endif // RAZORSYSSTATCONFIGURATION_H
