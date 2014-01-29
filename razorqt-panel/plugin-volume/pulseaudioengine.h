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

#ifndef PULSEAUDIOENGINE_H
#define PULSEAUDIOENGINE_H

#include "audioengine.h"

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtCore/QMap>

#include <pulse/pulseaudio.h>

// PA_VOLUME_UI_MAX is only supported since pulseaudio 0.9.23
#ifndef PA_VOLUME_UI_MAX
#define PA_VOLUME_UI_MAX (pa_sw_volume_from_dB(+11.0))
#endif

class AudioDevice;

class PulseAudioEngine : public AudioEngine
{
    Q_OBJECT

public:
    PulseAudioEngine(QObject *parent = 0);
    ~PulseAudioEngine();

    const QString backendName() const { return QLatin1String("PulseAudio"); }

    int volumeMax(AudioDevice */*device*/) const { return m_maximumVolume; }

public slots:
    void commitDeviceVolume(AudioDevice *device);
    void setMute(AudioDevice *device, bool state);
    void setIgnoreMaxVolume(bool ignore);

signals:
    void sinkInfoChanged(AudioDevice *device);
    void contextStateChanged(pa_context_state_t state);
    void readyChanged(bool ready);
    void retrieveSinksNeeded();

private slots:
    void handleContextStateChanged();
    void connectContext();
    void retrieveSinkInfo(AudioDevice *device);
    void retrieveSinks();

private:
    void setupSubscription();
    void setContextState(pa_context_state_t state);

    pa_mainloop_api *m_mainLoopApi;
    pa_threaded_mainloop *m_mainLoop;
    pa_context *m_context;

    pa_context_state_t m_contextState;
    bool m_ready;
    QTimer m_reconnectionTimer;
    int m_maximumVolume;

    QMap<AudioDevice *, pa_cvolume> m_cVolumeMap;

    void waitForOperation(pa_operation *operation);

    static void sinkInfoCallback(pa_context *context, const pa_sink_info *info, int isLast, void *userdata);
    void sinkInfoHandler(pa_context *context, const pa_sink_info *info, int isLast);

    static void contextSubscriptionCallback(pa_context *context, pa_subscription_event_type_t t, uint32_t idx, void *userdata);
    void contextSubscriptionHandler(pa_context *context, pa_subscription_event_type_t t, uint32_t idx);

    static void contextStateCallback(pa_context *context, void *userdata);
    void contextStateHandler(pa_context *context);

    static void contextEventCallback(pa_context *context, const char *name, pa_proplist *p, void *userdata);
    void contextEventHandler(pa_context *context, const char *name, pa_proplist *p);

    static void contextSuccessCallback(pa_context *context, int success, void *userdata);
    void contextSuccessHandler(pa_context *context, int success);
};

#endif // PULSEAUDIOENGINE_H
