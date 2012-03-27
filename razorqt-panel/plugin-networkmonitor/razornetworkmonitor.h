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

#ifndef RAZORNETWORKMONITOR_H
#define RAZORNETWORKMONITOR_H
#include "../panel/razorpanelplugin.h"
#include <QLabel>

/*!
  TODO: How to define cable is not connected?
  */
class RazorNetworkMonitor: public RazorPanelPlugin
{
	Q_OBJECT
public:
	RazorNetworkMonitor(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
	~RazorNetworkMonitor();

	virtual RazorPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }

protected:
	void virtual timerEvent(QTimerEvent *event);
	void virtual paintEvent ( QPaintEvent * event );
	void virtual resizeEvent(QResizeEvent *);

protected slots:
	virtual void showConfigureDialog();
	virtual void settigsChanged();

private:
	QString iconName(const QString& state) const
	{
		return QString(":/images/knemo-%1-%2.png")
				.arg(m_iconList[m_iconIndex], state );
	}

	QWidget m_stuff;

	QStringList m_iconList;

	int m_iconIndex;

	QString m_interface;
	QPixmap m_pic;

};

EXPORT_RAZOR_PANEL_PLUGIN_H

#endif // RAZORNETWORKMONITOR_H


