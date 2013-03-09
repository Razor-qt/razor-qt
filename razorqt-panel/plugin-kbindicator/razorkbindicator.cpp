/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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

#include "razorkbindicator.h"

#include <QtGui/QLabel>

#include "razorkbindicatoreventfilter.h"


Q_EXPORT_PLUGIN2(panelkbindicator, RazorKbIndicatorLibrary)


RazorKbIndicator::RazorKbIndicator(const IRazorPanelPluginStartupInfo &startupInfo):
    QObject(),
    IRazorPanelPlugin(startupInfo),
    content(new QLabel()),
    eventFilter(RazorKbIndicatorEventFilter::instance())
{
    connect(eventFilter, SIGNAL(indicatorsChanged(uint,uint)), this, SLOT(setIndicators(uint,uint)));
    content->setAlignment(Qt::AlignCenter);

    settingsChanged();
    content->setEnabled(eventFilter->getLockStatus(bit));
    realign();
}

RazorKbIndicator::~RazorKbIndicator()
{
    delete content;
}

QWidget *RazorKbIndicator::widget()
{
    return content;
}

void RazorKbIndicator::settingsChanged()
{
    bit = settings()->value("bit", 0).toInt();
    content->setText(settings()->value("text", QString("C")).toString());
    content->setEnabled(eventFilter->getLockStatus(bit));
}

QDialog *RazorKbIndicator::configureDialog()
{
    return new RazorKbIndicatorConfiguration(settings());
}

void RazorKbIndicator::realign()
{
    content->setMinimumSize(panel()->lineSize(), panel()->lineSize());
}

void RazorKbIndicator::setIndicators(unsigned int changed, unsigned int state)
{
    if (changed & (1 << bit))
        content->setEnabled(state & (1 << bit));
}
