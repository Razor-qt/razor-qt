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

#include "pulseaudioengine.h"

#include "audiodevice.h"

#include <QtCore/QMetaType>
#include <QtDebug>

//#define PULSEAUDIO_ENGINE_DEBUG

class MainLoopLocker
{
public:
    MainLoopLocker(pa_threaded_mainloop *mainloop)
        : m_mainloop(mainloop)
    {
        pa_threaded_mainloop_lock(mainloop);
    }

    ~MainLoopLocker()
    {
        pa_threaded_mainloop_unlock(m_mainloop);
    }

private:
    pa_threaded_mainloop *m_mainloop;
};

PulseAudioEngine::PulseAudioEngine(QObject *parent) :
    AudioEngine(parent),
    m_mainLoop(0),
    m_context(0),
    m_contextState(PA_CONTEXT_UNCONNECTED),
    m_ready(false),
    m_maximumVolume(PA_VOLUME_UI_MAX)
{
    qRegisterMetaType<pa_context_state_t>("pa_context_state_t");

    m_reconnectionTimer.setSingleShot(true);
    m_reconnectionTimer.setInterval(100);
    connect(&m_reconnectionTimer, SIGNAL(timeout()), this, SLOT(connectContext()));

    m_mainLoop = pa_threaded_mainloop_new();
    if (!m_mainLoop) {
        qWarning("Unable to create pulseaudio mainloop");
        return;
    }

    if (pa_threaded_mainloop_start(m_mainLoop)) {
        qWarning("Unable to start pulseaudio mainloop");
        pa_threaded_mainloop_free(m_mainLoop);
        m_mainLoop = 0;
        return;
    }

    m_mainLoopApi = pa_threaded_mainloop_get_api(m_mainLoop);

    connect(this, SIGNAL(contextStateChanged(pa_context_state_t)), this, SLOT(handleContextStateChanged()));

    connectContext();
}

PulseAudioEngine::~PulseAudioEngine()
{
    if (m_context) {
        pa_context_unref(m_context);
        m_context = 0;
    }

    if (m_mainLoop) {
        pa_threaded_mainloop_free(m_mainLoop);
        m_mainLoop = 0;
    }
}

void PulseAudioEngine::waitForOperation(pa_operation *operation)
{
    while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(m_mainLoop);
    pa_operation_unref(operation);
}

void PulseAudioEngine::commitDeviceVolume(AudioDevice *device)
{
    if (!device || !m_ready)
        return;

    pa_volume_t v = (device->volume() * m_maximumVolume) / 100;
    pa_cvolume tmpVolume = m_cVolumeMap.value(device);
    pa_cvolume *volume = pa_cvolume_set(&tmpVolume, tmpVolume.channels, v);

    MainLoopLocker locker(m_mainLoop); Q_UNUSED(locker)

    waitForOperation(((device->type() == Sink)
                      ? pa_context_set_sink_volume_by_index
                      : pa_context_set_source_volume_by_index) (m_context, device->index(), volume, contextSuccessCallback, this));
}

void PulseAudioEngine::contextSuccessCallback(pa_context *context, int success, void *userdata)
{
    reinterpret_cast<PulseAudioEngine*>(userdata)->contextSuccessHandler(context, success);
}

void PulseAudioEngine::contextSuccessHandler(pa_context *context, int success)
{
    Q_UNUSED(context)
    Q_UNUSED(success)

    pa_threaded_mainloop_signal(m_mainLoop, 0);
}

void PulseAudioEngine::retrieveSinks()
{
    if (!m_ready)
        return;

    MainLoopLocker locker(m_mainLoop); Q_UNUSED(locker)

    waitForOperation(pa_context_get_sink_info_list(m_context, &PulseAudioEngine::sinkInfoCallback, this));
}

void PulseAudioEngine::sinkInfoCallback(pa_context *context, const pa_sink_info *info, int isLast, void *userdata)
{
    reinterpret_cast<PulseAudioEngine*>(userdata)->sinkInfoHandler(context, info, isLast);
}

void PulseAudioEngine::sinkInfoHandler(pa_context *context, const pa_sink_info *info, int isLast)
{
//    QMap<pa_sink_state, QString> stateMap;
//    stateMap[PA_SINK_INVALID_STATE] = "n/a";
//    stateMap[PA_SINK_RUNNING] = "RUNNING";
//    stateMap[PA_SINK_IDLE] = "IDLE";
//    stateMap[PA_SINK_SUSPENDED] = "SUSPENDED";

    if (isLast < 0) {
        pa_threaded_mainloop_signal(m_mainLoop, 0);
        qWarning() << QString("Failed to get sink information: %1").arg(pa_strerror(pa_context_errno(context)));
        return;
    }

    if (isLast) {
        pa_threaded_mainloop_signal(m_mainLoop, 0);
        return;
    }


    AudioDevice *dev = 0;
    bool newSink = false;
    QString name = QString::fromUtf8(info->name);

    foreach (AudioDevice *device, m_sinks) {
        if (device->name() == name) {
            dev = device;
            break;
        }
    }

    if (!dev) {
        dev = new AudioDevice(Sink, this);
        newSink = true;
    }

    dev->setName(name);
    dev->setIndex(info->index);
    dev->setDescription(QString::fromUtf8(info->description));
    dev->setMuteNoCommit(info->mute);

    // TODO: save separately? alsa does not have it
    m_cVolumeMap.insert(dev, info->volume);

    pa_volume_t v = pa_cvolume_avg(&(info->volume));
    dev->setVolumeNoCommit((v * 100) / m_maximumVolume);

    if (newSink) {
        m_sinks.append(dev);
        emit sinkListChanged();
    }
}

void PulseAudioEngine::setupSubscription()
{
    if (!m_ready)
        return;

    connect(this, SIGNAL(sinkInfoChanged(AudioDevice*)), this, SLOT(retrieveSinkInfo(AudioDevice*)), Qt::QueuedConnection);
    pa_context_set_subscribe_callback(m_context, &PulseAudioEngine::contextSubscriptionCallback, this);

    MainLoopLocker locker(m_mainLoop); Q_UNUSED(locker)

    waitForOperation(pa_context_subscribe(m_context, PA_SUBSCRIPTION_MASK_SINK, contextSuccessCallback, this));
}

void PulseAudioEngine::contextSubscriptionCallback(pa_context *context, pa_subscription_event_type_t t, uint32_t idx, void *userdata)
{
    reinterpret_cast<PulseAudioEngine *>(userdata)->contextSubscriptionHandler(context, t, idx);
}

void PulseAudioEngine::contextSubscriptionHandler(pa_context *context, pa_subscription_event_type_t t, uint32_t idx)
{
    Q_UNUSED(context)
    Q_UNUSED(t)

    foreach (AudioDevice *dev, sinks()) {
        if (dev->index() == idx) {
            emit sinkInfoChanged(dev);
            break;
        }
    }
}

void PulseAudioEngine::handleContextStateChanged()
{
    if (m_contextState == PA_CONTEXT_FAILED || m_contextState == PA_CONTEXT_TERMINATED) {
        qWarning("Razor-Volume: Context connection failed or terminated, trying to reconnect");
        m_reconnectionTimer.start();
    }
}

void PulseAudioEngine::connectContext()
{
    m_reconnectionTimer.stop();

    if (!m_mainLoop)
        return;

    bool ok = false;
    {
        MainLoopLocker locker(m_mainLoop); Q_UNUSED(locker)

        if (m_context) {
            pa_context_unref(m_context);
            m_context = 0;
        }

        m_context = pa_context_new(m_mainLoopApi, "razor-volume");
        pa_context_set_state_callback(m_context, &PulseAudioEngine::contextStateCallback, this);
        pa_context_set_event_callback(m_context, &PulseAudioEngine::contextEventCallback, this);

        if (!m_context) {
            m_reconnectionTimer.start();
            return;
        }

        if (pa_context_connect(m_context, NULL, (pa_context_flags_t)0, NULL) < 0) {
            m_reconnectionTimer.start();
            return;
        }

        for (bool keepGoing = true; keepGoing;) {
            switch (m_contextState) {
                case PA_CONTEXT_CONNECTING:
                case PA_CONTEXT_AUTHORIZING:
                case PA_CONTEXT_SETTING_NAME:
                    break;

                case PA_CONTEXT_READY:
                    keepGoing = false;
                    ok = true;
                    break;

                case PA_CONTEXT_TERMINATED:
                    keepGoing = false;
                    break;

                case PA_CONTEXT_FAILED:
                default:
                    qWarning() << QString("Connection failure: %1").arg(pa_strerror(pa_context_errno(m_context)));
                    keepGoing = false;
            }

            if (keepGoing)
                pa_threaded_mainloop_wait(m_mainLoop);
        }

    } // end of locker scope

    if (ok) {
        retrieveSinks();
        setupSubscription();
    } else {
        m_reconnectionTimer.start();
    }
}

void PulseAudioEngine::contextStateCallback(pa_context *context, void *userdata)
{
    reinterpret_cast<PulseAudioEngine *>(userdata)->contextStateHandler(context);
}

void PulseAudioEngine::contextStateHandler(pa_context *context)
{
    // update internal state
    pa_context_state_t state = pa_context_get_state(context);
    setContextState(state);

#ifdef PULSEAUDIO_ENGINE_DEBUG
    switch (state) {
        case PA_CONTEXT_UNCONNECTED:
            qWarning("context unconnected");
            break;
        case PA_CONTEXT_CONNECTING:
            qWarning("context connecting");
            break;
        case PA_CONTEXT_AUTHORIZING:
            qWarning("context authorizing");
            break;
        case PA_CONTEXT_SETTING_NAME:
            qWarning("context setting name");
            break;
        case PA_CONTEXT_READY:
            qWarning("context ready");
            break;
        case PA_CONTEXT_FAILED:
            qWarning("context failed");
            break;
        case PA_CONTEXT_TERMINATED:
            qWarning("context terminated");
            break;
        default:
            qWarning("we should never hit this state");
    }
#endif

    pa_threaded_mainloop_signal(m_mainLoop, 0);
}

void PulseAudioEngine::contextEventCallback(pa_context *context, const char *name, pa_proplist *p, void *userdata)
{
    reinterpret_cast<PulseAudioEngine*>(userdata)->contextEventHandler(context, name, p);
}

void PulseAudioEngine::contextEventHandler(pa_context *context, const char *name, pa_proplist *p)
{
    Q_UNUSED(context)
    Q_UNUSED(name)
    Q_UNUSED(p)

#ifdef PULSEAUDIO_ENGINE_DEBUG
    char *human_p = pa_proplist_to_string(p);
    qWarning("event received %s, props: %s", name, human_p);
    pa_xfree(human_p);
#endif
}

void PulseAudioEngine::retrieveSinkInfo(AudioDevice *device)
{
    if (!m_ready)
        return;

    MainLoopLocker locker(m_mainLoop); Q_UNUSED(locker)

    waitForOperation(pa_context_get_sink_info_by_index(m_context, device->index(), &PulseAudioEngine::sinkInfoCallback, this));
}

void PulseAudioEngine::setMute(AudioDevice *device, bool state)
{
    if (!m_ready)
        return;

    MainLoopLocker locker(m_mainLoop); Q_UNUSED(locker)

    waitForOperation(pa_context_set_sink_mute_by_index(m_context, device->index(), state, contextSuccessCallback, this));
}

void PulseAudioEngine::setContextState(pa_context_state_t state)
{
    if (m_contextState == state)
        return;

    m_contextState = state;

    // update ready member as it depends on state
    if (m_ready == (m_contextState == PA_CONTEXT_READY))
        return;

    m_ready = (m_contextState == PA_CONTEXT_READY);

    emit contextStateChanged(m_contextState);
    emit readyChanged(m_ready);
}

void PulseAudioEngine::setIgnoreMaxVolume(bool ignore)
{
    if (ignore)
        m_maximumVolume = PA_VOLUME_UI_MAX;
    else
        m_maximumVolume = pa_sw_volume_from_dB(0);
    retrieveSinks(); // to update volumes
}
