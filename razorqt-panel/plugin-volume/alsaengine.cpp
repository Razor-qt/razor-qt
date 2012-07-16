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

#include "alsaengine.h"

#include "audiodevice.h"

#include <QtCore/QMetaType>
#include <QtDebug>

AlsaEngine::AlsaEngine(QObject *parent) :
    AudioEngine(parent)
{
    discoverDevices();
}

void AlsaEngine::commitDeviceVolume(AudioDevice *device)
{
}

void AlsaEngine::setMute(AudioDevice *device, bool state)
{
}

void AlsaEngine::discoverDevices()
{
    int error;
    int cardNum = -1;

    while (1) {
        error = snd_card_next(&cardNum);

        if (cardNum < 0)
            break;

        char   str[64];
        sprintf(str, "hw:%i", cardNum);

        snd_ctl_t *cardHandle;
        if ((error = snd_ctl_open(&cardHandle, str, 0)) < 0) {
           qWarning("Can't open card %i: %s\n", cardNum, snd_strerror(error));
           continue;
        }

        snd_ctl_card_info_t *cardInfo;
        snd_ctl_card_info_alloca(&cardInfo);

        if ((error = snd_ctl_card_info(cardHandle, cardInfo)) < 0) {
            qWarning("Can't get info for card %i: %s\n", cardNum, snd_strerror(error));
        } else {
            AudioDevice *dev = new AudioDevice(Sink, this, this);
            // TODO: check on encoding
            dev->name = QString::fromAscii(snd_ctl_card_info_get_name(cardInfo));
            dev->index = cardNum;
            dev->description = QString::fromAscii(snd_ctl_card_info_get_longname(cardInfo));

            m_sinks.append(dev);
        }

        snd_ctl_close(cardHandle);
    }

    snd_config_update_free_global();
}
