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
#include "razorcpuloadconfiguration.h"
#include <QtCore>
#include <QPainter>
#include <QLinearGradient>

extern "C" {
#include <statgrab.h>
}


EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorCpuLoad)

RazorCpuLoad::RazorCpuLoad(const RazorPanelPluginStartInfo* startInfo, QWidget* parent):
	RazorPanelPlugin(startInfo, parent),
	m_showText(false)
{
	setObjectName("CpuLoad");
	addWidget(&m_stuff);

	/* Initialise statgrab */
	sg_init();

	/* Drop setuid/setgid privileges. */
	if (sg_drop_privileges() != 0) {
		perror("Error. Failed to drop privileges");
	}

	m_font.setPointSizeF(8);
	startTimer(500);

	settigsChanged();
}

RazorCpuLoad::~RazorCpuLoad()
{
}

void RazorCpuLoad::resizeEvent(QResizeEvent *)
{
	m_stuff.setMinimumWidth(18);
	m_stuff.setMaximumWidth(18);
	m_stuff.setMinimumHeight(24);

	update();
}


double RazorCpuLoad::getLoadCpu() const
{
	sg_cpu_percents* cur = sg_get_cpu_percents();
	return (cur->user + cur->kernel + cur->nice);
}

void RazorCpuLoad::timerEvent(QTimerEvent *event)
{
	double avg = getLoadCpu();
	if ( qAbs(m_avg-avg)>1 )
	{
		m_avg = avg;
		setToolTip(tr("Cpu load %1%").arg(m_avg));
		update();
	}
}

void RazorCpuLoad::paintEvent ( QPaintEvent * )
{
	QPainter p(this);
	QPen pen;
	pen.setWidth(2);
	p.setPen(pen);
	p.setRenderHint(QPainter::Antialiasing, true);
	const double w = 20;

	p.setFont(m_font);
	QRectF r = rect();

	float vo = r.height()*(1-m_avg*0.01);
	float ho = (r.width() - w )/2.0;
	QRectF r1(r.left()+ho, r.top()+vo, r.width()-2*ho, r.height()-vo );

	QLinearGradient shade(0, 0, r1.width(), 0);
	shade.setSpread(QLinearGradient::ReflectSpread);
	shade.setColorAt(0, QColor(0, 196, 0, 128));
	shade.setColorAt(0.5, QColor(0, 128, 0, 255) );
	shade.setColorAt(1, QColor(0, 196, 0 , 128));


	p.fillRect(r1, shade);

	if( m_showText )
		p.drawText(rect(), Qt::AlignCenter, QString::number(m_avg));
}

void RazorCpuLoad::showConfigureDialog()
{
	RazorCpuLoadConfiguration *confWindow =
			this->findChild<RazorCpuLoadConfiguration*>("RazorCpuLoadConfigurationWindow");

	if (!confWindow)
	{
		confWindow = new RazorCpuLoadConfiguration(settings(), this);
	}

	confWindow->show();
	confWindow->raise();
	confWindow->activateWindow();
}

void RazorCpuLoad::settigsChanged()
{
	m_showText = settings().value("showText", false).toBool();
	update();
}

