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

#ifndef VOLUMEBUTTON_H
#define VOLUMEBUTTON_H

#include <QtGui/QToolButton>
#include <QtCore/QTimer>

class VolumePopup;
class RazorPanel;

class VolumeButton : public QToolButton
{
    Q_OBJECT
public:
    VolumeButton(RazorPanel *panel, QWidget* parent = 0);
    ~VolumeButton();

    void setShowOnClicked(bool state);
    void setMuteOnMiddleClick(bool state);
    void setMixerCommand(const QString &command);

    VolumePopup *volumePopup() const { return m_volumePopup; }

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void toggleVolumeSlider();
    void hideVolumeSlider();
    void showVolumeSlider();
    void handlePopupHideTimeout();
    void popupHideTimerStart();
    void popupHideTimerStop();
    void handleMixerLaunch();
    void handleStockIconChanged(const QString &iconName);

private:
    VolumePopup *m_volumePopup;
    RazorPanel *m_panel;
    QTimer m_popupHideTimer;
    int m_popupHideTimerDuration;
    bool m_showOnClick;
    bool m_muteOnMiddleClick;
    QString m_mixerCommand;
};

#endif // VOLUMEBUTTON_H
