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

#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <QtCore/QObject>

class AudioEngine;

typedef enum AudioDeviceType {
    Sink = 0,
    Source = 1,
    PulseAudioDeviceTypeLength
} AudioDeviceType;

class AudioDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(AudioDeviceType type READ type CONSTANT)

public:
    AudioDevice(AudioDeviceType t, AudioEngine *engine, QObject *parent = 0);
    ~AudioDevice();

    int volume() const { return m_volume; }
    bool mute() const { return m_mute; }
    AudioDeviceType type() const { return m_type; }
    const QString &name() const { return m_name; }
    const QString &description() const { return m_description; }
    uint index() const { return m_index; }

    void setName(const QString &name);
    void setDescription(const QString &description);
    void setIndex(uint index);

public slots:
    void setVolume(int volume);
    void setVolumeNoCommit(int volume);
    void toggleMute();
    void setMute(bool state);
    void setMuteNoCommit(bool state);

signals:
    void volumeChanged(int volume);
    void muteChanged(bool state);
    void nameChanged(const QString &name);
    void descriptionChanged(const QString &description);
    void indexChanged(uint index);

private:
    AudioEngine *m_engine;
    int m_volume;
    bool m_mute;
    AudioDeviceType m_type;
    QString m_name;
    uint m_index;
    QString m_description;
};

#endif // AUDIODEVICE_H
