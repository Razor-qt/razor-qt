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

#include <QtGui/QMessageBox>
#include <QtDebug>
#include <qtxdg/xdgicon.h>

#include "volumebutton.h"
#include "razorvolumeconfiguration.h"
#include "pulseaudioengine.h"
#include "pulseaudiodevice.h"

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

    m_paEngine = new PulseAudioEngine(this);
    connect(m_paEngine, SIGNAL(sinkListChanged()), this, SLOT(updateConfigurationSinkList()));
    connect(m_volumeButton, SIGNAL(volumeChanged(int)), this, SLOT(updateVolume(int)));
    updateConfigurationSinkList();
}

RazorVolume::~RazorVolume()
{
}

void RazorVolume::showConfigureDialog()
{
    m_configWindow->show();
    m_configWindow->raise();
    m_configWindow->activateWindow();
}

void RazorVolume::settingsChanged()
{
    m_defaultSinkIndex = settings().value("defaultSink", 0).toInt();
    if (m_paEngine->sinks().at(m_defaultSinkIndex))
        m_defaultSink = m_paEngine->sinks().at(m_defaultSinkIndex);

    qWarning() << "settings changed" << m_defaultSinkIndex;
}

void RazorVolume::updateConfigurationSinkList()
{
    m_configWindow->setSinkList(m_paEngine->sinks());
    settingsChanged();
}

void RazorVolume::updateVolume(int value)
{
    if (!m_defaultSink)
        return;

    m_defaultSink->setVolume(value);
    m_defaultSink->commitVolume();
}
