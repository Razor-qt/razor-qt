/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Christian Surlykke <christian@surlykke.dk>
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QButtonGroup>
#include "razorqt/razorsettings.h"

namespace Ui
{
    class Settings;
}

#define LIDCLOSEDACTION_KEY "lidClosedAction"
#define POWERLOWACTION_KEY "powerLowAction"
enum
{
    NOTHING,
    SLEEP,
    HIBERNATE
};

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

public slots:
    virtual void accept();
    virtual void showEvent(QShowEvent *);

private:
    Ui::Settings *ui;
    QButtonGroup lidClosedGroup;
    QButtonGroup powerLowGroup;
    RazorSettings settings;
};

#endif // SETTINGS_H
