/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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

#include "razornetworkmonitor.h"
#include "razornetworkmonitorconfiguration.h"
#include <QtCore/QEvent>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QLinearGradient>

extern "C" {
#include <statgrab.h>
}

EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorNetworkMonitor)

RazorNetworkMonitor::RazorNetworkMonitor(const RazorPanelPluginStartInfo* startInfo, QWidget* parent):
    RazorPanelPlugin(startInfo, parent)
{
    setObjectName("NetworkMonitor");
    addWidget(&m_stuff);

    /* Initialise statgrab */
    sg_init();

    m_iconList << "modem" << "monitor"
               << "network" << "wireless";

    startTimer(800);

    settingsChanged();
}

RazorNetworkMonitor::~RazorNetworkMonitor()
{
}

void RazorNetworkMonitor::resizeEvent(QResizeEvent *)
{
    m_stuff.setMinimumWidth(m_pic.width() + 2);
    m_stuff.setMinimumHeight(m_pic.height() + 2);

    update();
}


void RazorNetworkMonitor::timerEvent(QTimerEvent *event)
{
    bool matched = false;

    int num_network_stats;

    sg_network_io_stats *network_stats = sg_get_network_io_stats_diff(&num_network_stats);

    for (int x = 0; x < num_network_stats; x++)
    {
        if (m_interface == QString::fromLocal8Bit(network_stats->interface_name))
        {
            if (network_stats->rx != 0 && network_stats->tx != 0)
            {
                m_pic.load(iconName("transmit-receive"));
            }
            else if (network_stats->rx != 0 && network_stats->tx == 0)
            {
                m_pic.load(iconName("receive"));
            }
            else if (network_stats->rx == 0 && network_stats->tx != 0)
            {
                m_pic.load(iconName("transmit"));
            }
            else
            {
                m_pic.load(iconName("idle"));
            }

            matched = true;

            break;
        }

        network_stats++;
    }

    if (!matched)
    {
        m_pic.load(iconName("error"));
    }

    update();
}

void RazorNetworkMonitor::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QRectF r = rect();

    int leftOffset = (r.width() - m_pic.width() + 2) / 2;
    int topOffset = (r.height() - m_pic.height() + 2) / 2;

    p.drawPixmap(leftOffset, topOffset, m_pic);
}

bool RazorNetworkMonitor::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        int num_network_stats;
        sg_network_io_stats *network_stats = sg_get_network_io_stats(&num_network_stats);
        for (int x = 0; x < num_network_stats; x++)
        {
            if (m_interface == QString::fromLocal8Bit(network_stats->interface_name))
            {
                setToolTip(tr("Network interface <b>%1</b>").arg(m_interface) + "<br>"
                           + tr("Transmitted %1").arg(convertUnits(network_stats->tx)) + "<br>"
                           + tr("Received %1").arg(convertUnits(network_stats->rx))
                          );
            }
            network_stats++;
        }
    }
    return RazorPanelPlugin::event(event);
}

void RazorNetworkMonitor::showConfigureDialog()
{
    RazorNetworkMonitorConfiguration *confWindow =
        this->findChild<RazorNetworkMonitorConfiguration*>("RazorNetworkMonitorConfigurationWindow");

    if (!confWindow)
    {
        confWindow = new RazorNetworkMonitorConfiguration(settings(), this);
    }

    confWindow->show();
    confWindow->raise();
    confWindow->activateWindow();
}

void RazorNetworkMonitor::settingsChanged()
{
    m_iconIndex = settings().value("icon", 1).toInt();
    m_interface = settings().value("interface").toString();
    if (m_interface.isEmpty())
    {
        int count;
        sg_network_iface_stats* stats = sg_get_network_iface_stats(&count);
        if (count > 0)
            m_interface = QString(stats[0].interface_name);
    }

    m_pic.load(iconName("error"));
}

QString RazorNetworkMonitor::convertUnits(double num)
{
    QString unit = tr("B");
    QStringList units = QStringList(tr("KiB")) << tr("MiB") << tr("GiB") << tr("TiB") << tr("PiB");
    for (QStringListIterator iter(units); num >= 1024 && iter.hasNext();)
    {
        num /= 1024;
        unit = iter.next();
    }
    return QString::number(num, 'f', 2) + " " + unit;
}
