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

#include "pulseaudiodevice.h"

#include "pulseaudioengine.h"

PulseAudioDevice::PulseAudioDevice(PulseAudioEngine *engine, QObject *parent) :
    QObject(parent),
    m_engine(engine),
    m_volume(0)
{
}

PulseAudioDevice::~PulseAudioDevice()
{
}

// this is just for setting the internal volume
void PulseAudioDevice::setVolumeNoCommit(int volume)
{
    if (m_volume == volume)
        return;

    m_volume = volume;
    emit volumeChanged(m_volume);
}

// this performs a volume change on the device
void PulseAudioDevice::setVolume(int volume)
{
    if (m_volume == volume)
        return;

    m_volume = volume;

    if (m_engine)
        m_engine->commitDeviceVolume(this);
}
