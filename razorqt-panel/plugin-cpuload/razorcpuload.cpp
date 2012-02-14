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

#include "razorcpuload.h"
#include <QtCore>
#include <QPainter>
#include <QLinearGradient>

EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorCpuLoad)

RazorCpuLoad::RazorCpuLoad(const RazorPanelPluginStartInfo* startInfo, QWidget* parent):
	RazorPanelPlugin(startInfo, parent)
{
	setObjectName("CpuLoad");
	addWidget(&m_stuff);

	getLoadCpu();

	startTimer(500);
}

RazorCpuLoad::~RazorCpuLoad()
{
}

void RazorCpuLoad::resizeEvent(QResizeEvent *)
{
	if( panel()->isHorizontal() )
	{
		m_stuff.setMinimumWidth(m_stuff.height() * 0.5);
		m_stuff.setMinimumHeight(0);
	} else
	{
		m_stuff.setMinimumHeight(m_stuff.width() * 2.0);
		m_stuff.setMinimumWidth(0);
	}
}


void RazorCpuLoad::getLoadCpu()
{
	QFile f("/proc/stat");
	f.open(QIODevice::ReadOnly);
	QTextStream st(&f);
	QStringList lines = st.readAll().split("\n");

	for( int i = 0 ; i< lines.count(); i++)
	{
		const QString& line = lines[i];
		if( line.startsWith("cpu "))
		{
			QStringList params = line.split(QRegExp("[ \t]+"));
			quint64 oldUser		= currentUser;
			quint64 oldNice		= currentNice;
			quint64 oldSystem	= currentSystem;
			quint64 oldIdle		= currentIdle;

			currentUser = params[1].toInt();
			currentNice = params[2].toInt();
			currentSystem= params[3].toInt();
			currentIdle	= params[4].toInt();

			quint64 deltaUser = currentUser - oldUser;
			quint64 deltaNice = currentNice - oldNice;
			quint64 deltaSystem= currentSystem - oldSystem;
			quint64 deltaIdle = currentIdle - oldIdle;

			quint64 total = deltaUser+deltaNice+deltaSystem+deltaIdle;
			m_avg = 100 * (deltaUser+deltaNice+deltaSystem)/total;

			setToolTip(tr("Cpu load %1%").arg(m_avg));

			break;
		}
	}
}

void RazorCpuLoad::timerEvent(QTimerEvent *event)
{
	getLoadCpu();
	update();
}

void RazorCpuLoad::paintEvent ( QPaintEvent * )
{
	QPainter p(this);
	QPen pen;
	pen.setWidth(2);
	p.setPen(pen);
	p.setRenderHint(QPainter::Antialiasing, true);

	QLinearGradient shade(0, 0, 0, height());
	shade.setColorAt(0, Qt::red);
	shade.setColorAt(1, Qt::green);

	float o = rect().height()*(1-m_avg*0.01);
	QRectF r = rect();

	QRectF r1(r.left(), r.top()+o, r.width(), r.height()-o );
	p.fillRect(r1, shade);
	p.drawText(rect(), Qt::AlignCenter, QString::number(m_avg));
}

