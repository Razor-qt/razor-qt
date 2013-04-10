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
#ifdef USE_PULSEAUDIO
#include "pulseaudioengine.h"
#endif
#ifdef USE_ALSA
#include "alsaengine.h"
#endif

#include <QtGui/QMessageBox>
#include <QtDebug>
#include <qtxdg/xdgicon.h>
#include <razor-global-key-shortcuts-client/razor-global-key-shortcuts-client.h>
#include <razorqt/razornotification.h>

Q_EXPORT_PLUGIN2(volume, RazorVolumePluginLibrary)

#define DEFAULT_UP_SHORTCUT "XF86AudioRaiseVolume"
#define DEFAULT_DOWN_SHORTCUT "XF86AudioLowerVolume"
#define DEFAULT_MUTE_SHORTCUT "XF86AudioMute"

RazorVolume::RazorVolume(const IRazorPanelPluginStartupInfo &startupInfo):
        QObject(),
        IRazorPanelPlugin(startupInfo),
        m_engine(0),
        m_defaultSinkIndex(0),
        m_defaultSink(0)
{
    m_volumeButton = new VolumeButton(this);

    m_notification = new RazorNotification("", this);

    // global key shortcuts
    QString shortcutNotRegistered;

    m_keyVolumeUp = GlobalKeyShortcut::Client::instance()->addAction(QString(), QString("/panel/%1/volume_control/%2/up").arg(QFileInfo(settings()->fileName()).baseName()).arg(settings()->group()), tr("Increase sound volume"), this);
    if (m_keyVolumeUp)
    {
        connect(m_keyVolumeUp, SIGNAL(activated()), this, SLOT(handleShortcutVolumeUp()));

        if (m_keyVolumeUp->shortcut().isEmpty())
        {
            m_keyVolumeUp->changeShortcut(DEFAULT_UP_SHORTCUT);
            if (m_keyVolumeUp->shortcut().isEmpty())
            {
                shortcutNotRegistered = " '" DEFAULT_UP_SHORTCUT "'";
            }
        }
    }

    m_keyVolumeDown = GlobalKeyShortcut::Client::instance()->addAction(QString(), QString("/panel/%1/volume_control/%2/down").arg(QFileInfo(settings()->fileName()).baseName()).arg(settings()->group()), tr("Decrease sound volume"), this);
    if (m_keyVolumeDown)
    {
        connect(m_keyVolumeDown, SIGNAL(activated()), this, SLOT(handleShortcutVolumeDown()));

        if (m_keyVolumeDown->shortcut().isEmpty())
        {
            m_keyVolumeDown->changeShortcut(DEFAULT_DOWN_SHORTCUT);
            if (m_keyVolumeDown->shortcut().isEmpty())
            {
                shortcutNotRegistered += " '" DEFAULT_DOWN_SHORTCUT "'";
            }
        }
    }

    m_keyMuteToggle = GlobalKeyShortcut::Client::instance()->addAction(QString(), QString("/panel/%1/volume_control/%2/mute").arg(QFileInfo(settings()->fileName()).baseName()).arg(settings()->group()), tr("Mute/unmute sound volume"), this);
    if (m_keyMuteToggle)
    {
        connect(m_keyMuteToggle, SIGNAL(activated()), this, SLOT(handleShortcutVolumeMute()));

        if (m_keyMuteToggle->shortcut().isEmpty())
        {
            m_keyMuteToggle->changeShortcut(DEFAULT_MUTE_SHORTCUT);
            if (m_keyMuteToggle->shortcut().isEmpty())
            {
                shortcutNotRegistered += " '" DEFAULT_MUTE_SHORTCUT "'";
            }
        }
    }

    if(!shortcutNotRegistered.isEmpty())
    {
        m_notification->setSummary(tr("Volume Control: The following shortcuts can not be registered: %1").arg(shortcutNotRegistered));
        m_notification->update();
    }
    
    m_notification->setTimeout(1000);
    m_notification->setUrgencyHint(RazorNotification::UrgencyLow);

    settingsChanged();
}

RazorVolume::~RazorVolume()
{
    delete m_volumeButton;
}

void RazorVolume::setAudioEngine(AudioEngine *engine)
{
    if (m_engine) {
        if (m_engine->backendName() == engine->backendName())
            return;

        m_volumeButton->volumePopup()->setDevice(0);

        disconnect(m_engine, 0, 0, 0);
        delete m_engine;
        m_engine = 0;
    }

    m_engine = engine;
    connect(m_engine, SIGNAL(sinkListChanged()), this, SLOT(updateConfigurationSinkList()));

    updateConfigurationSinkList();
}


void RazorVolume::settingsChanged()
{
#if defined(USE_PULSEAUDIO) && defined(USE_ALSA)
    if (!m_engine || m_engine->backendName() != settings()->value(SETTINGS_AUDIO_ENGINE, SETTINGS_DEFAULT_AUDIO_ENGINE).toString()) {
        if (settings()->value(SETTINGS_AUDIO_ENGINE, SETTINGS_DEFAULT_AUDIO_ENGINE).toString() == "PulseAudio")
            setAudioEngine(new PulseAudioEngine(this));
        else
            setAudioEngine(new AlsaEngine(this));
    }
#elif defined(USE_PULSEAUDIO)
    if (!m_engine)
        setAudioEngine(new PulseAudioEngine(this));
#elif defined(USE_ALSA)
    if (!m_engine)
        setAudioEngine(new AlsaEngine(this));
#endif

    m_volumeButton->setShowOnClicked(settings()->value(SETTINGS_SHOW_ON_LEFTCLICK, SETTINGS_DEFAULT_SHOW_ON_LEFTCLICK).toBool());
    m_volumeButton->setMuteOnMiddleClick(settings()->value(SETTINGS_MUTE_ON_MIDDLECLICK, SETTINGS_DEFAULT_MUTE_ON_MIDDLECLICK).toBool());
    m_volumeButton->setMixerCommand(settings()->value(SETTINGS_MIXER_COMMAND, SETTINGS_DEFAULT_MIXER_COMMAND).toString());
    m_volumeButton->volumePopup()->setSliderStep(settings()->value(SETTINGS_STEP, SETTINGS_DEFAULT_STEP).toInt());

    m_defaultSinkIndex = settings()->value(SETTINGS_DEVICE, SETTINGS_DEFAULT_DEVICE).toInt();
    if (m_engine && m_engine->sinks().count() > 0) {
        m_defaultSinkIndex = qBound(0, m_defaultSinkIndex, m_engine->sinks().count()-1);

        m_defaultSink = m_engine->sinks().at(m_defaultSinkIndex);
        m_volumeButton->volumePopup()->setDevice(m_defaultSink);

        m_engine->setIgnoreMaxVolume(settings()->value(SETTINGS_IGNORE_MAX_VOLUME, SETTINGS_DEFAULT_IGNORE_MAX_VOLUME).toBool());
    }
}

void RazorVolume::updateConfigurationSinkList()
{

}

void RazorVolume::handleShortcutVolumeUp()
{
    if (m_defaultSink)
    {
        m_defaultSink->setVolume(m_defaultSink->volume() + settings()->value(SETTINGS_STEP, SETTINGS_DEFAULT_STEP).toInt());
        m_notification->setSummary(tr("Volume: %1").arg(QString::number(m_defaultSink->volume())));
        m_notification->update();
    }
}

void RazorVolume::handleShortcutVolumeDown()
{
    if (m_defaultSink)
    {
        m_defaultSink->setVolume(m_defaultSink->volume() - settings()->value(SETTINGS_STEP, SETTINGS_DEFAULT_STEP).toInt());
        m_notification->setSummary(tr("Volume: %1").arg(QString::number(m_defaultSink->volume())));
        m_notification->update();
    }
}

void RazorVolume::handleShortcutVolumeMute()
{
    if (m_defaultSink)
        m_defaultSink->toggleMute();
}

QWidget *RazorVolume::widget()
{
    return m_volumeButton;
}

void RazorVolume::realign()
{
    m_volumeButton->hideVolumeSlider();
}

QDialog *RazorVolume::configureDialog()
{
    RazorVolumeConfiguration *configWindow = new RazorVolumeConfiguration(*settings());
    configWindow->setAttribute(Qt::WA_DeleteOnClose, true);

    if (m_engine)
       configWindow->setSinkList(m_engine->sinks());

    return configWindow;
}

#undef DEFAULT_UP_SHORTCUT
#undef DEFAULT_DOWN_SHORTCUT
#undef DEFAULT_MUTE_SHORTCUT
