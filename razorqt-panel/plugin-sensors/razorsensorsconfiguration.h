/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Maciej Płaza <plaza.maciej@gmail.com>
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


#ifndef RAZORSENSORSCONFIGURATION_H
#define RAZORSENSORSCONFIGURATION_H

#include <QtGui/QDialog>
#include <QtGui/QAbstractButton>
#include <QtGui/QButtonGroup>
#include <QtCore/QLocale>
#include <QtCore/QDateTime>

#include "razorqt/razorsettings.h"

namespace Ui {
    class RazorSensorsConfiguration;
}

class RazorSensorsConfiguration : public QDialog
{
    Q_OBJECT

public:
    explicit RazorSensorsConfiguration(QSettings *settings, QWidget *parent = 0);
    ~RazorSensorsConfiguration();

private:
    Ui::RazorSensorsConfiguration *ui;
    QSettings *mSettings;
    RazorSettingsCache oldSettings;

    /*
      Read settings from conf file and put data into controls.
    */
    void loadSettings();

private slots:
    /*
      Saves settings in conf file.
    */
    void saveSettings();
    void dialogButtonsAction(QAbstractButton *btn);
    void changeProgressBarColor();
    void detectedChipSelected(int index);
};

#endif // RAZORSENSORSCONFIGURATION_H
