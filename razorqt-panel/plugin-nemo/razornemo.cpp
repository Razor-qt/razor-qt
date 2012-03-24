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

#include "razornemo.h"
#include "razornemoconfiguration.h"
#include <QtCore>
#include <QPainter>
#include <QPixmap>
#include <QLinearGradient>

extern "C" {
#include <statgrab.h>
}

EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorNemo)

RazorNemo::RazorNemo(const RazorPanelPluginStartInfo* startInfo, QWidget* parent):
	RazorPanelPlugin(startInfo, parent)
{
	setObjectName("Nemo");
	addWidget(&m_stuff);

	/* Initialise statgrab */
	sg_init();

	/* Drop setuid/setgid privileges. */
	if (sg_drop_privileges() != 0) {
		perror("Error. Failed to drop privileges");
	}


	m_iconList << "modem" << "monitor"
			   << "network" << "wireless";

	m_stateList << "error" << "offline"
				<< "transmit" << "idle"
				<< "receive" << "transmit-receive";

	startTimer(500);

	setToolTip( "hello" );

	settigsChanged();
}

RazorNemo::~RazorNemo()
{
}

void RazorNemo::resizeEvent(QResizeEvent *)
{
	m_stuff.setMinimumWidth(m_pic.width()+2);
	m_stuff.setMinimumHeight(m_pic.height()+2);

	update();
}


void RazorNemo::timerEvent(QTimerEvent *event)
{
	bool matched = false;

	int num_network_stats;

	sg_network_io_stats *network_stats = sg_get_network_io_stats_diff(&num_network_stats);

	for(int x = 0; x < num_network_stats; x++)
	{
		if ( m_interface == QString::fromLocal8Bit(network_stats->interface_name) )
		{
			setToolTip(QString("matched %1 %2").arg(network_stats->tx).arg(network_stats->rx) );
			if ( network_stats->rx != 0 && network_stats->tx != 0 )
			{
				m_pic.load( iconName(5) );
			} else if ( network_stats->rx != 0 && network_stats->tx == 0 )
			{
				m_pic.load( iconName(4) );
			} else if ( network_stats->rx == 0 && network_stats->tx != 0 )
			{
				m_pic.load( iconName(2) );
			} else
			{
				m_pic.load( iconName(3) );
			}

			matched = true;
		}

		/* Move the pointer onto the next interface. Since this returns a static buffer, we dont need
		 * to keep track of the orginal pointer to free later */
		network_stats++;
	}

	if( !matched)
		m_pic.load( iconName(0) );

	update();
}

void RazorNemo::paintEvent ( QPaintEvent * )
{
	QPainter p(this);

	QRectF r = rect();

	int leftOffset = (r.width() - m_pic.width() + 2) / 2;
	int topOffset = (r.height() - m_pic.height() + 2) / 2;

	p.drawPixmap(leftOffset, topOffset, m_pic);
}

void RazorNemo::showConfigureDialog()
{
	RazorNemoConfiguration *confWindow =
			this->findChild<RazorNemoConfiguration*>("RazorNemoConfigurationWindow");

	if (!confWindow)
	{
		confWindow = new RazorNemoConfiguration(settings(), this);
	}

	confWindow->show();
	confWindow->raise();
	confWindow->activateWindow();
}

void RazorNemo::settigsChanged()
{
	m_iconIndex = settings().value("icon", 0).toInt();
	m_interface = settings().value("interface1", "").toString();

	m_pic.load( QString(":/images/knemo-%1-%2.png")
				.arg(m_iconList[m_iconIndex], m_stateList[3] ) );

	update();
}

