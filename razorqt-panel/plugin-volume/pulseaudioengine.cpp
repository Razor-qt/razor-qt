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

#include "pulseaudiodevice.h"

#include <QtDebug>

//#define PULSEAUDIO_ENGINE_DEBUG

static void sinkInfoCallback(pa_context *context, const pa_sink_info *info, int isLast, void *userdata)
{
    PulseAudioEngine *pulseEngine = static_cast<PulseAudioEngine*>(userdata);
    QMap<pa_sink_state, QString> stateMap;
    stateMap[PA_SINK_INVALID_STATE] = "n/a";
    stateMap[PA_SINK_RUNNING] = "RUNNING";
    stateMap[PA_SINK_IDLE] = "IDLE";
    stateMap[PA_SINK_SUSPENDED] = "SUSPENDED";

    if (isLast < 0) {
        qWarning() << QString("Failed to get sink information: %s").arg(pa_strerror(pa_context_errno(context)));
        return;
    }

    if (isLast) {
        pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
        return;
    }

    pulseEngine->addOrUpdateSink(info);
}

static void contextEventCallback(pa_context *context, const char *name, pa_proplist *p, void *userdata)
{
#ifdef PULSEAUDIO_ENGINE_DEBUG
    qWarning("event received %s", name);
#endif
}

static void contextStateCallback(pa_context *context, void *userdata)
{
    PulseAudioEngine *pulseEngine = reinterpret_cast<PulseAudioEngine*>(userdata);

    // update internal state
    pa_context_state_t state = pa_context_get_state(context);
    pulseEngine->setContextState(state);

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

    pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
}

static void contextSuccessCallback(pa_context *context, int success, void *userdata)
{
    Q_UNUSED(context);
    Q_UNUSED(success);
    Q_UNUSED(userdata);

    PulseAudioEngine *pulseEngine = reinterpret_cast<PulseAudioEngine*>(userdata);
    pa_threaded_mainloop_signal(pulseEngine->mainloop(), 0);
}

static void contextSubscriptionCallback(pa_context *context, pa_subscription_event_type_t t, uint32_t idx, void *userdata)
{
    PulseAudioEngine *pulseEngine = reinterpret_cast<PulseAudioEngine*>(userdata);
    foreach (PulseAudioDevice *dev, pulseEngine->sinks()) {
        if (dev->index == idx) {
            pulseEngine->requestSinkInfoUpdate(dev);
            break;
        }
    }
}


PulseAudioEngine::PulseAudioEngine(QObject *parent) :
    QObject(parent),
    m_contextState(PA_CONTEXT_UNCONNECTED),
    m_ready(false)
{
    m_mainLoop = pa_threaded_mainloop_new();
    if (m_mainLoop == 0) {
        qWarning("Unable to create pulseaudio mainloop");
        return;
    }

    if (pa_threaded_mainloop_start(m_mainLoop) != 0) {
        qWarning("Unable to start pulseaudio mainloop");
        pa_threaded_mainloop_free(m_mainLoop);
        return;
    }

    m_mainLoopApi = pa_threaded_mainloop_get_api(m_mainLoop);

    pa_threaded_mainloop_lock(m_mainLoop);

    m_context = pa_context_new(m_mainLoopApi, "razor-volume");
    pa_context_set_state_callback(m_context, contextStateCallback, this);
    pa_context_set_event_callback(m_context, contextEventCallback, this);

    if (!m_context) {
        qWarning("Unable to create new pulseaudio context");
        pa_threaded_mainloop_free(m_mainLoop);
        return;
    }

    if (pa_context_connect(m_context, NULL, (pa_context_flags_t)0, NULL) < 0) {
        qWarning("Unable to create a connection to the pulseaudio context");
        pa_context_unref(m_context);
        pa_threaded_mainloop_free(m_mainLoop);
        return;
    }

    bool keepGoing = true;
    bool ok = true;

    while (keepGoing) {
        switch (m_contextState) {
            case PA_CONTEXT_CONNECTING:
            case PA_CONTEXT_AUTHORIZING:
            case PA_CONTEXT_SETTING_NAME:
                break;

            case PA_CONTEXT_READY:
                keepGoing = false;
                break;

            case PA_CONTEXT_TERMINATED:
                keepGoing = false;
                ok = false;
                break;

            case PA_CONTEXT_FAILED:
            default:
                qCritical() << QString("Connection failure: %1").arg(pa_strerror(pa_context_errno(m_context)));
                keepGoing = false;
                ok = false;
        }

        if (keepGoing) {
            pa_threaded_mainloop_wait(m_mainLoop);
        }
    }

    pa_threaded_mainloop_unlock(m_mainLoop);

    if (ok) {
        retrieveSinks();
        setupSubscription();
    } else {
        pa_context_unref(m_context);
        m_context = 0;
    }
}

PulseAudioEngine::~PulseAudioEngine()
{
    qDeleteAll(m_sinks);
}

void PulseAudioEngine::addOrUpdateSink(const pa_sink_info *info)
{
    PulseAudioDevice *dev = 0;
    bool newSink = false;

    foreach (PulseAudioDevice *device, m_sinks) {
        if (info->name == device->name) {
            dev = device;
            break;
        }
    }

    if (!dev) {
        dev = new PulseAudioDevice(Sink, this);
        newSink = true;
    }

    dev->name = info->name;
    dev->index = info->index;
    dev->description = info->description;
    dev->cvolume = info->volume;
    dev->setMute(info->mute);

    pa_volume_t v = pa_cvolume_avg(&(info->volume));
    dev->setVolumeNoCommit(((double)v*100.0) / PA_VOLUME_UI_MAX);

    if (newSink) {
        m_sinks.append(dev);
        emit sinkListChanged();
    }
}

const QList<PulseAudioDevice *> &PulseAudioEngine::sinks() const
{
    return m_sinks;
}

pa_threaded_mainloop *PulseAudioEngine::mainloop() const
{
    return m_mainLoop;
}

void PulseAudioEngine::requestSinkInfoUpdate(PulseAudioDevice *device)
{
    emit sinkInfoChanged(device);
}

void PulseAudioEngine::commitDeviceVolume(PulseAudioDevice *device)
{
    if (!device || !m_ready)
        return;

    pa_volume_t v = (device->volume()/100.0) * PA_VOLUME_UI_MAX;
    pa_cvolume *volume = pa_cvolume_set(&(device->cvolume), device->cvolume.channels, v);

    pa_threaded_mainloop_lock(m_mainLoop);

    pa_operation *operation;
    if (device->type() == Sink)
        operation = pa_context_set_sink_volume_by_index(m_context, device->index, volume, contextSuccessCallback, this);
    else
        operation = pa_context_set_source_volume_by_index(m_context, device->index, volume, contextSuccessCallback, this);

    while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(m_mainLoop);
    pa_operation_unref(operation);

    pa_threaded_mainloop_unlock(m_mainLoop);
}

void PulseAudioEngine::retrieveSinks()
{
    if (!m_ready)
        return;

    pa_threaded_mainloop_lock(m_mainLoop);

    pa_operation *operation;
    operation = pa_context_get_sink_info_list(m_context, sinkInfoCallback, this);
    while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(m_mainLoop);
    pa_operation_unref(operation);

    pa_threaded_mainloop_unlock(m_mainLoop);
}

void PulseAudioEngine::setupSubscription()
{
    if (!m_ready)
        return;

    connect(this, SIGNAL(sinkInfoChanged(PulseAudioDevice*)), this, SLOT(retrieveSinkInfo(PulseAudioDevice*)), Qt::QueuedConnection);
    pa_context_set_subscribe_callback(m_context, contextSubscriptionCallback, this);

    pa_threaded_mainloop_lock(m_mainLoop);

    pa_operation *operation;
    operation = pa_context_subscribe(m_context, PA_SUBSCRIPTION_MASK_SINK, contextSuccessCallback, this);
    while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(m_mainLoop);
    pa_operation_unref(operation);

    pa_threaded_mainloop_unlock(m_mainLoop);
}

void PulseAudioEngine::retrieveSinkInfo(PulseAudioDevice *device)
{
    if (!m_ready)
        return;

    pa_threaded_mainloop_lock(m_mainLoop);

    pa_operation *operation;
    operation = pa_context_get_sink_info_by_index(m_context, device->index, sinkInfoCallback, this);
    while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(m_mainLoop);
    pa_operation_unref(operation);

    pa_threaded_mainloop_unlock(m_mainLoop);
}

void PulseAudioEngine::mute(PulseAudioDevice *device)
{
    setMute(device, true);
}

void PulseAudioEngine::unmute(PulseAudioDevice *device)
{
    setMute(device, false);
}

void PulseAudioEngine::setMute(PulseAudioDevice *device, bool state)
{
    if (!m_ready)
        return;

    pa_threaded_mainloop_lock(m_mainLoop);

    pa_operation *operation;
    operation = pa_context_set_sink_mute_by_index(m_context, device->index, state, contextSuccessCallback, this);
    while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(m_mainLoop);
    pa_operation_unref(operation);

    pa_threaded_mainloop_unlock(m_mainLoop);
}

void PulseAudioEngine::setContextState(pa_context_state_t state)
{
    if (m_contextState == state)
        return;

    m_contextState = state;
    emit contextStateChanged(m_contextState);

    // just used internally
    m_ready = (m_contextState == PA_CONTEXT_READY);
}



