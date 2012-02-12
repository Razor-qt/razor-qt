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
#include <QtGui/QMessageBox>
#include <QtCore>
#include <QPainter>
#include <QLinearGradient>

EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorCpuLoad)

RazorCpuLoad::RazorCpuLoad(const RazorPanelPluginStartInfo* startInfo, QWidget* parent):
		RazorPanelPlugin(startInfo, parent)
{
	setObjectName("CpuLoad");
	addWidget(&m_label);
	m_label.setText("     ");

	getLoadCpu();

	startTimer(500);
//	setMinimumWidth(250);
//	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

RazorCpuLoad::~RazorCpuLoad()
{
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
			quint64 U0 = U1;
			quint64 N0 = N1;
			quint64 S0 = S1;
			quint64 I0 = I1;

			U1 = params[1].toInt();
			N1 = params[2].toInt();
			S1 = params[3].toInt();
			I1 = params[4].toInt();

			quint64 Ud = U1 - U0;
			quint64 Nd = N1 - N0;
			quint64 Sd = S1 - S0;
			quint64 Id = I1 - I0;

			quint64 total = Ud + Nd + Sd + Id;
			avg = 100 * (Ud + Nd + Sd)/total;

			break;
		}
	}
}

void RazorCpuLoad::timerEvent(QTimerEvent *event)
{
	getLoadCpu();
	update();
}

void RazorCpuLoad::paintEvent ( QPaintEvent * event )
{
	QPainter p(this);
	QPen pen;
	pen.setWidth(2);
	p.setPen(pen);
	p.setRenderHint(QPainter::Antialiasing, true);

	QLinearGradient shade(0, 0, 0, height());
	shade.setColorAt(0, Qt::red);
	shade.setColorAt(1, Qt::green);

	float o = rect().height()*(1-avg*0.01);
//	QMessageBox::information(this, "error", QString::number(o));
	QRect r = rect();

	QRect r1(r.left(), r.top()+o, r.width(), r.height()-o );
	p.fillRect(r1, shade);
	p.drawText(rect(), Qt::AlignCenter, QString::number(avg));
}

