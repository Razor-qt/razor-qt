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

#include "alsadevice.h"

#include <QtCore/QMetaType>
#include <QtCore/QSocketNotifier>
#include <QtDebug>

AlsaEngine *AlsaEngine::m_instance = 0;

static int alsa_elem_event_callback(snd_mixer_elem_t *elem, unsigned int mask)
{
    AlsaEngine *engine = AlsaEngine::instance();
    if (engine)
        engine->updateDevice(engine->getDeviceByAlsaElem(elem));

    return 0;
}

static int alsa_mixer_event_callback(snd_mixer_t *mixer, unsigned int mask, snd_mixer_elem_t *elem)
{
    return 0;
}

AlsaEngine::AlsaEngine(QObject *parent) :
    AudioEngine(parent)
{
    discoverDevices();
    m_instance = this;
}

AlsaEngine *AlsaEngine::instance()
{
    return m_instance;
}

int AlsaEngine::volumeMax(AudioDevice *device) const
{
    AlsaDevice *dev = qobject_cast<AlsaDevice*>(device);
    if (!dev || !dev->element())
        return 100;

    long vmin;
    long vmax;
    snd_mixer_selem_get_playback_volume_range(dev->element(), &vmin, &vmax);

    return vmax;
}

AlsaDevice *AlsaEngine::getDeviceByAlsaElem(snd_mixer_elem_t *elem) const
{
    foreach (AudioDevice *device, m_sinks) {
        AlsaDevice *dev = qobject_cast<AlsaDevice*>(device);
        if (!dev || !dev->element())
            continue;

        if (dev->element() == elem)
            return dev;
    }

    return 0;
}

void AlsaEngine::commitDeviceVolume(AudioDevice *device)
{
    AlsaDevice *dev = qobject_cast<AlsaDevice*>(device);
    if (!dev || !dev->element())
        return;

    long value = dev->volume();
    snd_mixer_selem_set_playback_volume_all(dev->element(), value);
}

void AlsaEngine::setMute(AudioDevice *device, bool state)
{
    AlsaDevice *dev = qobject_cast<AlsaDevice*>(device);
    if (!dev || !dev->element())
        return;

    if (snd_mixer_selem_has_playback_switch(dev->element()))
        snd_mixer_selem_set_playback_switch_all(dev->element(), (int)!state);
    else if (state)
        dev->setVolume(0);
}

void AlsaEngine::updateDevice(AlsaDevice *device)
{
    if (!device)
        return;

    long value;
    snd_mixer_selem_get_playback_volume(device->element(), (snd_mixer_selem_channel_id_t)0, &value);
    device->setVolumeNoCommit(value);

    if (snd_mixer_selem_has_playback_switch(device->element())) {
        int mute;
        snd_mixer_selem_get_playback_switch(device->element(), (snd_mixer_selem_channel_id_t)0, &mute);
        device->setMuteNoCommit(!(bool)mute);
    }
}

void AlsaEngine::driveAlsaEventHandling(int fd)
{
    snd_mixer_handle_events(m_mixerMap.value(fd));
}

void AlsaEngine::discoverDevices()
{
    int error;
    int cardNum = -1;

    while (1) {
        error = snd_card_next(&cardNum);

        if (cardNum < 0)
            break;

        char str[64];
        sprintf(str, "hw:%i", cardNum);

        snd_ctl_t *cardHandle;
        if ((error = snd_ctl_open(&cardHandle, str, 0)) < 0) {
            qWarning("Can't open card %i: %s\n", cardNum, snd_strerror(error));
            continue;
        }

        snd_ctl_card_info_t *cardInfo;
        snd_ctl_card_info_alloca(&cardInfo);

        QString cardName = QString::fromAscii(snd_ctl_card_info_get_name(cardInfo));
        if (cardName.isEmpty())
            cardName = QString::fromAscii(str);

        if ((error = snd_ctl_card_info(cardHandle, cardInfo)) < 0) {
            qWarning("Can't get info for card %i: %s\n", cardNum, snd_strerror(error));
        } else {
            // setup mixer and iterate over channels
            snd_mixer_t *mixer = 0;
            snd_mixer_open(&mixer, 0);
            snd_mixer_attach(mixer, str);
            snd_mixer_selem_register(mixer, NULL, NULL);
            snd_mixer_load(mixer);

            // setup event handler for mixer
            snd_mixer_set_callback(mixer, alsa_mixer_event_callback);

            // setup eventloop handling
            struct pollfd pfd;
            if (snd_mixer_poll_descriptors(mixer, &pfd, 1)) {
                QSocketNotifier *notifier = new QSocketNotifier(pfd.fd, QSocketNotifier::Read, this);
                connect(notifier, SIGNAL(activated(int)), this, SLOT(driveAlsaEventHandling(int)));
                m_mixerMap.insert(pfd.fd, mixer);
            }

            snd_mixer_elem_t *mixerElem = 0;
            mixerElem = snd_mixer_first_elem(mixer);

            while (mixerElem) {
                // check if we have a Sink or Source
                if (snd_mixer_selem_has_playback_volume(mixerElem)) {
                    AlsaDevice *dev = new AlsaDevice(Sink, this, this);
                    dev->setName(QString::fromAscii(snd_mixer_selem_get_name(mixerElem)));
                    dev->setIndex(cardNum);
                    dev->setDescription(cardName + " - " + dev->name());

                    // set alsa specific members
                    dev->setCardName(QString::fromAscii(str));
                    dev->setMixer(mixer);
                    dev->setElement(mixerElem);

                    long value;
                    snd_mixer_selem_get_playback_volume(mixerElem, (snd_mixer_selem_channel_id_t)0, &value);
                    dev->setVolumeNoCommit(value);

                    if (snd_mixer_selem_has_playback_switch(mixerElem)) {
                        int mute;
                        snd_mixer_selem_get_playback_switch(mixerElem, (snd_mixer_selem_channel_id_t)0, &mute);
                        dev->setMuteNoCommit(!(bool)mute);
                    }

                    // register event callback
                    snd_mixer_elem_set_callback(mixerElem, alsa_elem_event_callback);

                    m_sinks.append(dev);
                }

                mixerElem = snd_mixer_elem_next(mixerElem);
            }
        }

        snd_ctl_close(cardHandle);
    }

    snd_config_update_free_global();
}
