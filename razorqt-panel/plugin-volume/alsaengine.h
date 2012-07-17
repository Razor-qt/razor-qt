/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Johannes Zellner <webmaster@nebulon.de>
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

#ifndef ALSAENGINE_H
#define ALSAENGINE_H

#include "audioengine.h"

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QTimer>

#include <alsa/asoundlib.h>

class AlsaDevice;
class QSocketNotifier;

class AlsaEngine : public AudioEngine
{
    Q_OBJECT

public:
    AlsaEngine(QObject *parent = 0);

    int volumeMax(AudioDevice *device) const;

public slots:
    void commitDeviceVolume(AudioDevice *device);
    void setMute(AudioDevice *device, bool state);

private slots:
    void driveAlsaEventHandling(int fd);

private:
    void discoverDevices();
    QMap<int, snd_mixer_t *> m_mixerMap;
};

#endif // ALSAENGINE_H
