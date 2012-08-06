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

#include "volumebutton.h"

#include "volumepopup.h"
#include "audiodevice.h"

#include <QtGui/QSlider>
#include <QtGui/QMouseEvent>
#include <QtCore/QProcess>

#include <qtxdg/xdgicon.h>
#include "../panel/razorpanel.h"

VolumeButton::VolumeButton(RazorPanel *panel, QWidget* parent):
        QToolButton(parent),
        m_panel(panel),
        m_popupHideTimerDuration(1000),
        m_showOnClick(true),
        m_muteOnMiddleClick(true)
{
    m_volumePopup = new VolumePopup();

    connect(this, SIGNAL(clicked()), this, SLOT(toggleVolumeSlider()));
    connect(m_panel, SIGNAL(positionChanged()), this, SLOT(hideVolumeSlider()));
    connect(&m_popupHideTimer, SIGNAL(timeout()), this, SLOT(handlePopupHideTimeout()));

    connect(m_volumePopup, SIGNAL(mouseEntered()), this, SLOT(popupHideTimerStop()));
    connect(m_volumePopup, SIGNAL(mouseLeft()), this, SLOT(popupHideTimerStart()));

    connect(m_volumePopup, SIGNAL(deviceChanged()), this, SLOT(handleDeviceChanged()));
    connect(m_volumePopup, SIGNAL(launchMixer()), this, SLOT(handleMixerLaunch()));
    connect(m_volumePopup, SIGNAL(stockIconChanged(QString)), this, SLOT(handleStockIconChanged(QString)));
}

VolumeButton::~VolumeButton()
{
    if (m_volumePopup)
        delete m_volumePopup;
}

void VolumeButton::setShowOnClicked(bool state)
{
    if (m_showOnClick == state)
        return;

    m_showOnClick = state;
}

void VolumeButton::setMuteOnMiddleClick(bool state)
{
    m_muteOnMiddleClick = state;
}

void VolumeButton::setMixerCommand(const QString &command)
{
    m_mixerCommand = command;
}

void VolumeButton::enterEvent(QEvent *event)
{
    if (!m_showOnClick)
        showVolumeSlider();

    popupHideTimerStop();
}

void VolumeButton::leaveEvent(QEvent *event)
{
    popupHideTimerStart();
}

void VolumeButton::wheelEvent(QWheelEvent *event)
{
    m_volumePopup->handleWheelEvent(event);
}

void VolumeButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton && m_muteOnMiddleClick) {
        if (m_volumePopup->device()) {
            m_volumePopup->device()->toggleMute();
            return;
        }
    }

    QToolButton::mouseReleaseEvent(event);
}

void VolumeButton::toggleVolumeSlider()
{
    if (m_volumePopup->isVisible()) {
        popupHideTimerStop();
        handlePopupHideTimeout();
    } else {
        showVolumeSlider();
    }
}

void VolumeButton::showVolumeSlider()
{
    if (m_volumePopup->isVisible())
        return;

    popupHideTimerStop();
    m_volumePopup->updateGeometry();

    if (isLeftToRight())
    {
        switch (m_panel->position())
        {
        case RazorPanel::PositionTop:
            m_volumePopup->open(mapToGlobal(geometry().bottomLeft()), Qt::TopLeftCorner);
            break;

        case RazorPanel::PositionBottom:
            m_volumePopup->open(mapToGlobal(geometry().topLeft()), Qt::BottomLeftCorner);
            break;

        case RazorPanel::PositionLeft:
            m_volumePopup->open(mapToGlobal(geometry().topRight()), Qt::TopLeftCorner);
            break;

        case RazorPanel::PositionRight:
            m_volumePopup->open(mapToGlobal(geometry().topLeft()), Qt::TopLeftCorner);
            break;
        }
    }
    else
    {
        switch (m_panel->position())
        {
        case RazorPanel::PositionTop:
            m_volumePopup->open(mapToGlobal(geometry().bottomRight()), Qt::TopRightCorner);
            break;

        case RazorPanel::PositionBottom:
            m_volumePopup->open(mapToGlobal(geometry().topRight()), Qt::BottomRightCorner);
            break;

        case RazorPanel::PositionLeft:
            m_volumePopup->open(mapToGlobal(geometry().topRight()), Qt::TopLeftCorner);
            break;

        case RazorPanel::PositionRight:
            m_volumePopup->open(mapToGlobal(geometry().topLeft()), Qt::TopLeftCorner);
            break;
        }
    }
}

void VolumeButton::hideVolumeSlider()
{
    popupHideTimerStart();
}

void VolumeButton::handlePopupHideTimeout()
{
    m_volumePopup->hide();
}

void VolumeButton::popupHideTimerStart()
{
    m_popupHideTimer.start(m_popupHideTimerDuration);
}

void VolumeButton::popupHideTimerStop()
{
    m_popupHideTimer.stop();
}

void VolumeButton::handleMixerLaunch()
{
    QProcess::startDetached(m_mixerCommand);
}

void VolumeButton::handleStockIconChanged(const QString &iconName)
{
    setIcon(XdgIcon::fromTheme(iconName));
}
