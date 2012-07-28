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

#ifndef VOLUMEPOPUP_H
#define VOLUMEPOPUP_H

#include <QtGui/QWidget>

class QSlider;
class QPushButton;
class AudioDevice;

class VolumePopup : public QWidget
{
    Q_OBJECT
public:
    VolumePopup(QWidget* parent = 0);

    void open(QPoint pos, Qt::Corner anchor);
    void handleWheelEvent(QWheelEvent *event);

    QSlider *volumeSlider() const { return m_volumeSlider; }

    AudioDevice *device() const { return m_device; }
    void setDevice(AudioDevice *device);

signals:
    void mouseEntered();
    void mouseLeft();

    void volumeChanged(int value);
    void deviceChanged();
    void launchMixer();

protected:
    void resizeEvent(QResizeEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private slots:
    void handleSliderValueChanged(int value);
    void handleMuteToggleClicked();

private:
    void realign();

    QSlider *m_volumeSlider;
    QPushButton *m_mixerButton;
    QPushButton *m_muteToggleButton;
    QPoint m_pos;
    Qt::Corner m_anchor;
    AudioDevice *m_device;
};

#endif // VOLUMEPOPUP_H
