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

#ifndef RAZORVOLUMECONFIGURATION_H
#define RAZORVOLUMECONFIGURATION_H

#include "../panel/razorpanelpluginconfigdialog.h"

#include <QtCore/QList>

#define SETTINGS_MIXER_COMMAND          "mixerCommand"
#define SETTINGS_SHOW_ON_LEFTCLICK      "showOnLeftClick"
#define SETTINGS_MUTE_ON_MIDDLECLICK    "showOnMiddleClick"
#define SETTINGS_DEVICE                 "device"
#define SETTINGS_STEP                   "volumeAdjustStep"
#define SETTINGS_IGNORE_MAX_VOLUME      "ignoreMaxVolume"

#define SETTINGS_DEFAULT_SHOW_ON_LEFTCLICK      true
#define SETTINGS_DEFAULT_MUTE_ON_MIDDLECLICK    true
#define SETTINGS_DEFAULT_DEVICE                 0
#define SETTINGS_DEFAULT_STEP                   3
#ifdef USE_PULSEAUDIO
    #define SETTINGS_DEFAULT_MIXER_COMMAND      "pavucontrol"
#else
    #define SETTINGS_DEFAULT_MIXER_COMMAND      "qasmixer"
#endif
#define SETTINGS_DEFAULT_IGNORE_MAX_VOLUME      false

class AudioDevice;

namespace Ui {
    class RazorVolumeConfiguration;
}

class RazorVolumeConfiguration : public RazorPanelPluginConfigDialog
{
    Q_OBJECT

public:
    explicit RazorVolumeConfiguration(QSettings &settings, QWidget *parent = 0);
    ~RazorVolumeConfiguration();

public slots:
    void setSinkList(const QList<AudioDevice*> sinks);
    void sinkSelectionChanged(int index);
    void showOnClickedChanged(bool state);
    void muteOnMiddleClickChanged(bool state);
    void mixerLineEditChanged(const QString &command);
    void stepSpinBoxChanged(int step);
    void ignoreMaxVolumeCheckBoxChanged(bool state);

protected slots:
    virtual void loadSettings();

private:
    Ui::RazorVolumeConfiguration *ui;
};

#endif // RAZORVOLUMECONFIGURATION_H
