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

#ifndef PULSEAUDIODEVICE_H
#define PULSEAUDIODEVICE_H

#include <QtCore/QObject>

#include <pulse/pulseaudio.h>

class PulseAudioEngine;

typedef enum PulseAudioDeviceType {
    Sink = 0,
    Source = 1,
    PulseAudioDeviceTypeLength
} PulseAudioDeviceType;

class PulseAudioDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(PulseAudioDeviceType type READ type CONSTANT)

public:
    PulseAudioDevice(PulseAudioDeviceType t, PulseAudioEngine *engine, QObject *parent = 0);
    ~PulseAudioDevice();

    int volume() const { return m_volume; }
    bool mute() const { return m_mute; }
    PulseAudioDeviceType type() const { return m_type; }

public slots:
    void setVolume(int volume);
    void setVolumeNoCommit(int volume);
    void toggleMute();
    void setMute(bool state);

signals:
    void volumeChanged(int volume);
    void muteChanged();

public:
    QString name;
    uint index;
    QString description;
    pa_cvolume cvolume;

private:
    PulseAudioEngine *m_engine;
    int m_volume;
    bool m_mute;
    PulseAudioDeviceType m_type;
};

#endif // PULSEAUDIODEVICE_H
