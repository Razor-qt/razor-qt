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

#include "razorvolume.h"

#include "volumebutton.h"
#include "volumepopup.h"
#include "razorvolumeconfiguration.h"
#include "audiodevice.h"
#include "pulseaudioengine.h"
#include "alsaengine.h"

#include <QtGui/QMessageBox>
#include <QtDebug>
#include <qtxdg/xdgicon.h>
#include <razorqxt/qxtglobalshortcut.h>
#include <razorqt/razornotification.h>

EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorVolume)

RazorVolume::RazorVolume(const RazorPanelPluginStartInfo* startInfo, QWidget* parent):
        RazorPanelPlugin(startInfo, parent),
        m_defaultSinkIndex(0),
        m_defaultSink(0)
{
    setObjectName("Volume");

    m_volumeButton = new VolumeButton(panel(), this);
    addWidget(m_volumeButton);

    m_configWindow = new RazorVolumeConfiguration(settings(), this);

    // global key shortcuts
    m_keyVolumeUp = new QxtGlobalShortcut(this);
    m_keyVolumeDown = new QxtGlobalShortcut(this);
    m_keyMuteToggle = new QxtGlobalShortcut(this);

    QKeySequence keySequenceVolumeUp(Qt::Key_VolumeUp);
    if (!m_keyVolumeUp->setShortcut(keySequenceVolumeUp)) {
        RazorNotification::notify(tr("Show Desktop: Global shortcut '%1' cannot be registered").arg(keySequenceVolumeUp.toString()));
    }

    QKeySequence keySequenceVolumeDown(Qt::Key_VolumeDown);
    if (!m_keyVolumeDown->setShortcut(keySequenceVolumeDown)) {
        RazorNotification::notify(tr("Show Desktop: Global shortcut '%1' cannot be registered").arg(keySequenceVolumeDown.toString()));
    }

    QKeySequence keySequenceMuteToggle(Qt::Key_VolumeMute);
    if (!m_keyMuteToggle->setShortcut(keySequenceMuteToggle)) {
        RazorNotification::notify(tr("Show Desktop: Global shortcut '%1' cannot be registered").arg(keySequenceMuteToggle.toString()));
    }

    connect(m_keyVolumeUp, SIGNAL(activated()), this, SLOT(handleShortcutVolumeUp()));
    connect(m_keyVolumeDown, SIGNAL(activated()), this, SLOT(handleShortcutVolumeDown()));
    connect(m_keyMuteToggle, SIGNAL(activated()), this, SLOT(handleShortcutVolumeMute()));

#ifdef USE_PULSEAUDIO
    m_engine = new PulseAudioEngine(this);
#else
    m_engine = new AlsaEngine(this);
#endif

    connect(m_engine, SIGNAL(sinkListChanged()), this, SLOT(updateConfigurationSinkList()));
    updateConfigurationSinkList();
}

void RazorVolume::showConfigureDialog()
{
    m_configWindow->show();
    m_configWindow->raise();
    m_configWindow->activateWindow();
}

void RazorVolume::settingsChanged()
{
    m_defaultSinkIndex = settings().value(SETTINGS_DEVICE, SETTINGS_DEFAULT_DEVICE).toInt();
    if (m_engine->sinks().at(m_defaultSinkIndex)) {
        m_defaultSink = m_engine->sinks().at(m_defaultSinkIndex);
        m_volumeButton->volumePopup()->setDevice(m_defaultSink);
    }

    m_volumeButton->setShowOnClicked(settings().value(SETTINGS_SHOW_ON_LEFTCLICK, SETTINGS_DEFAULT_SHOW_ON_LEFTCLICK).toBool());
    m_volumeButton->setMuteOnMiddleClick(settings().value(SETTINGS_MUTE_ON_MIDDLECLICK, SETTINGS_DEFAULT_MUTE_ON_MIDDLECLICK).toBool());
    m_volumeButton->setMixerCommand(settings().value(SETTINGS_MIXER_COMMAND, SETTINGS_DEFAULT_MIXER_COMMAND).toString());
}

void RazorVolume::updateConfigurationSinkList()
{
    m_configWindow->setSinkList(m_engine->sinks());
    settingsChanged();
}

void RazorVolume::handleShortcutVolumeUp()
{
    if (m_defaultSink)
        m_defaultSink->setVolume(m_defaultSink->volume() + settings().value(SETTINGS_STEP, SETTINGS_DEFAULT_STEP).toInt());
}

void RazorVolume::handleShortcutVolumeDown()
{
    if (m_defaultSink)
        m_defaultSink->setVolume(m_defaultSink->volume() - settings().value(SETTINGS_STEP, SETTINGS_DEFAULT_STEP).toInt());
}

void RazorVolume::handleShortcutVolumeMute()
{
    if (m_defaultSink)
        m_defaultSink->toggleMute();
}
