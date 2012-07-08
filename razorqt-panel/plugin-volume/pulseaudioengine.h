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

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QTimer>

#include <pulse/pulseaudio.h>

class PulseAudioDevice;

class PulseAudioEngine : public QObject
{
    Q_OBJECT

public:
    PulseAudioEngine(QObject *parent = 0);
    ~PulseAudioEngine();

    void requestSinkInfoUpdate(PulseAudioDevice *device);
    void addOrUpdateSink(const pa_sink_info *info);

    const QList<PulseAudioDevice *> &sinks() const { return m_sinks; }
    pa_context_state_t contextState() const { return m_contextState; }
    bool ready() const { return m_ready; }
    pa_threaded_mainloop *mainloop() const { return m_mainLoop; }

public slots:
    void commitDeviceVolume(PulseAudioDevice *device);
    void retrieveSinkInfo(PulseAudioDevice *device);
    void mute(PulseAudioDevice *device);
    void unmute(PulseAudioDevice *device);
    void setMute(PulseAudioDevice *device, bool state);
    void setContextState(pa_context_state_t state);

signals:
    void sinkListChanged();
    void sinkInfoChanged(PulseAudioDevice *device);
    void contextStateChanged(pa_context_state_t state);
    void readyChanged(bool ready);

private slots:
    void handleContextStateChanged();
    void connectContext();

private:
    void retrieveSinks();
    void setupSubscription();

    pa_mainloop_api *m_mainLoopApi;
    pa_threaded_mainloop *m_mainLoop;
    pa_context *m_context;

    QList<PulseAudioDevice*> m_sinks;

    pa_context_state_t m_contextState;
    bool m_ready;
    QTimer m_reconnectionTimer;
};

#endif // PULSEAUDIOENGINE_H
