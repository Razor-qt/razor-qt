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

#include "razorsysstat.h"
#include "razorsysstatutils.h"

#include <sysstat/cpustat.hpp>
#include <sysstat/memstat.hpp>
#include <sysstat/netstat.hpp>

#include <QtCore/QTimer>
#include <QtCore/qmath.h>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtGui/QApplication>


EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorSysStat)

RazorSysStat::RazorSysStat(const RazorPanelPluginStartInfo *startInfo, QWidget *parent):
    RazorPanelPlugin(startInfo, parent),
    content(new RazorSysStatContent(*this, this))
{
    setObjectName("SysStat");

    addWidget(content);

    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->setSpacing(0);

    // has to be postponed to update the size first
    QTimer::singleShot(0, this, SLOT(settingsChanged()));
}

RazorSysStat::~RazorSysStat()
{
}

void RazorSysStat::settingsChanged()
{
    content->updateSettings(settings());
}

void RazorSysStat::showConfigureDialog()
{
    RazorSysStatConfiguration *confWindow = this->findChild<RazorSysStatConfiguration*>("SysStatConfigurationWindow");

    if (!confWindow)
    {
        confWindow = new RazorSysStatConfiguration(settings(), this);
    }

    confWindow->show();
    confWindow->raise();
    confWindow->activateWindow();
}

RazorSysStatContent::RazorSysStatContent(RazorSysStat &plugin_, QWidget *parent):
    QWidget(parent),
    plugin(plugin_),
    stat(NULL),
    historyLength(0),
    titleFontPixelHeight(0),
    historyOffset(0)
{

}

RazorSysStatContent::~RazorSysStatContent()
{

}

void RazorSysStatContent::updateSettings(const QSettings &settings)
{
    QString old_dataType = dataType;

    bgColour = QColor(settings.value("graph/bgColour", "0x000000").toString().toInt(NULL, 0) | 0xff000000);
    updateInterval = settings.value("graph/updateInterval", 1.0).toDouble();
    historyLength = settings.value("graph/historyLength", 30).toInt();

    gridLines = settings.value("grid/lines", 1).toInt();
    gridColour = QColor(settings.value("grid/colour", "0xc0c0c0").toString().toInt(NULL, 0) | 0xff000000);

    titleLabel = settings.value("title/label", QString()).toString();
    QFont defaultFont(QApplication::font());
    titleFont = QFont(
        settings.value("title/font/family", defaultFont.family()).toString(),
        settings.value("title/font/pointSize", defaultFont.pointSize()).toInt(),
        settings.value("title/font/weight", defaultFont.weight()).toInt(),
        settings.value("title/font/italic", defaultFont.italic()).toBool() );
    titleColour = QColor(settings.value("title/colour", "0xffffff").toString().toInt(NULL, 0) | 0xff000000);

    dataType = settings.value("data/type", QString("CPU")).toString();

    dataSource = settings.value("data/source", QString()).toString();

    cpuSystemColour = QColor(settings.value("cpu/systemColour",    "0x800000").toString().toInt(NULL, 0) | 0xff000000);
    cpuUserColour =   QColor(settings.value("cpu/userColour",      "0x000080").toString().toInt(NULL, 0) | 0xff000000);
    cpuNiceColour =   QColor(settings.value("cpu/niceColour",      "0x008000").toString().toInt(NULL, 0) | 0xff000000);
    cpuOtherColour =  QColor(settings.value("cpu/otherColour",     "0x808000").toString().toInt(NULL, 0) | 0xff000000);
    useFrequency = settings.value("cpu/useFrequency", true).toBool();
    frequencyColour = QColor(settings.value("cpu/frequencyColour", "0x60c0c0").toString().toInt(NULL, 0) | 0xff000000);

    memAppsColour =    QColor(settings.value("mem/appsColour",    "0x000080").toString().toInt(NULL, 0) | 0xff000000);
    memBuffersColour = QColor(settings.value("mem/buffersColour", "0x008000").toString().toInt(NULL, 0) | 0xff000000);
    memCachedColour =  QColor(settings.value("mem/cachedColour",  "0x808000").toString().toInt(NULL, 0) | 0xff000000);
    swapUsedColour =   QColor(settings.value("mem/swapColour",    "0x800000").toString().toInt(NULL, 0) | 0xff000000);

    netReceivedColour =    QColor(settings.value("net/receivedColour",    "0x000080").toString().toInt(NULL, 0) | 0xff000000);
    netTransmittedColour = QColor(settings.value("net/transmittedColour", "0x808000").toString().toInt(NULL, 0) | 0xff000000);
    netMaximumSpeed = PluginSysStat::netSpeedFromString(settings.value("net/maximumSpeed", "1 MB/s").toString());
    logarithmicScale = settings.value("net/logarithmicScale", true).toBool();

    logScaleSteps = settings.value("net/logarithmicScaleSteps", 4).toInt();
    logScaleMax = static_cast<qreal>(static_cast<int64_t>(1) << logScaleSteps);

    netRealMaximumSpeed = static_cast<qreal>(static_cast<int64_t>(1) << netMaximumSpeed);

    QColor netReceivedColour_hsv = netReceivedColour.toHsv();
    QColor netTransmittedColour_hsv = netTransmittedColour.toHsv();
    qreal hue = (netReceivedColour_hsv.hueF() + netTransmittedColour_hsv.hueF()) / 2;
    if (qAbs(netReceivedColour_hsv.hueF() - netTransmittedColour_hsv.hueF()) > 0.5)
        hue += 0.5;
    netBothColour.setHsvF(
        hue,
        (netReceivedColour_hsv.saturationF() + netTransmittedColour_hsv.saturationF()) / 2,
        (netReceivedColour_hsv.valueF()      + netTransmittedColour_hsv.valueF()     ) / 2 );


    if (titleLabel.isEmpty())
        titleFontPixelHeight = 0;
    else
    {
        QFontMetrics fm(titleFont);
        titleFontPixelHeight = fm.height();
    }

    if (stat)
        stat->stopUpdating();

    if (old_dataType != dataType)
    {
        if (stat)
            stat->deleteLater();

        if (dataType == "CPU")
            stat = new SysStat::CpuStat(this);
        else if (dataType == "Memory")
            stat = new SysStat::MemStat(this);
        else if (dataType == "Network")
            stat = new SysStat::NetStat(this);
    }
    else if (stat)
    {
        stat->disconnect(this);
    }

    if (stat)
    {
        if (dataType == "CPU")
        {
            if (useFrequency)
            {
                qobject_cast<SysStat::CpuStat*>(stat)->setMonitoring(SysStat::CpuStat::LoadAndFrequency);
                connect(qobject_cast<SysStat::CpuStat*>(stat), SIGNAL(update(float, float, float, float, float, uint)), this, SLOT(cpuUpdate(float, float, float, float, float, uint)));
            }
            else
            {
                qobject_cast<SysStat::CpuStat*>(stat)->setMonitoring(SysStat::CpuStat::LoadOnly);
                connect(qobject_cast<SysStat::CpuStat*>(stat), SIGNAL(update(float, float, float, float)), this, SLOT(cpuUpdate(float, float, float, float)));
            }
        }
        else if (dataType == "Memory")
        {
            if (dataSource == "memory")
                connect(qobject_cast<SysStat::MemStat*>(stat), SIGNAL(memoryUpdate(float, float, float)), this, SLOT(memoryUpdate(float, float, float)));
            else
                connect(qobject_cast<SysStat::MemStat*>(stat), SIGNAL(swapUpdate(float)), this, SLOT(swapUpdate(float)));
        }
        else if (dataType == "Network")
        {
            connect(qobject_cast<SysStat::NetStat*>(stat), SIGNAL(update(unsigned, unsigned)), this, SLOT(networkUpdate(unsigned, unsigned)));
        }

        stat->setUpdateInterval(updateInterval * 1000.0);
        stat->setMonitoredSource(dataSource);
    }

    reset();
}

void RazorSysStatContent::resizeEvent(QResizeEvent *event)
{
    reset();
}

void RazorSysStatContent::reset(void)
{
    setMinimumSize(plugin.panel()->isHorizontal() ? historyLength : 0, plugin.panel()->isHorizontal() ? 0 : historyLength);

    historyOffset = 0;
    historyImage[0] = QImage(historyLength, height(), QImage::Format_RGB32);
    historyImage[1] = QImage(historyLength, height(), QImage::Format_RGB32);
    historyImage[0].fill(bgColour);
    historyImage[1].fill(bgColour);

    update();
}

void RazorSysStatContent::drawLine(int index, int x, int y1, int y2, QColor clr)
{
    for (int y = y1; y < y2; ++y)
        historyImage[index].setPixel(x, y, clr.rgb());
}

void RazorSysStatContent::cpuUpdate(float user, float nice, float system, float other, float frequencyRate, uint)
{
    int imageIndex = historyOffset / historyLength;

    int x = historyOffset % historyLength;

    int height = historyImage[imageIndex].height() - titleFontPixelHeight;
    int y_system = height   - height * system * frequencyRate + 0.5;
    int y_user   = y_system - height * user   * frequencyRate + 0.5;
    int y_nice   = y_user   - height * nice   * frequencyRate + 0.5;
    int y_other  = y_nice   - height * other  * frequencyRate + 0.5;
    int y_freq   = height   - height * frequencyRate + 0.5;

    drawLine(imageIndex, x, 0,        y_freq,   bgColour);
    drawLine(imageIndex, x, y_freq,   y_other,  frequencyColour);
    drawLine(imageIndex, x, y_other,  y_nice,   cpuOtherColour);
    drawLine(imageIndex, x, y_nice,   y_user,   cpuNiceColour);
    drawLine(imageIndex, x, y_user,   y_system, cpuUserColour);
    drawLine(imageIndex, x, y_system, height,   cpuSystemColour);

    ++historyOffset;
    if (historyOffset == historyLength * 2)
        historyOffset = 0;

    update();
}

void RazorSysStatContent::cpuUpdate(float user, float nice, float system, float other)
{
    int imageIndex = historyOffset / historyLength;

    int x = historyOffset % historyLength;

    int height = historyImage[imageIndex].height() - titleFontPixelHeight;
    int y_system = height   - height * system + 0.5;
    int y_user   = y_system - height * user   + 0.5;
    int y_nice   = y_user   - height * nice   + 0.5;
    int y_other  = y_nice   - height * other  + 0.5;

    drawLine(imageIndex, x, 0,        y_other,  bgColour);
    drawLine(imageIndex, x, y_other,  y_nice,   cpuOtherColour);
    drawLine(imageIndex, x, y_nice,   y_user,   cpuNiceColour);
    drawLine(imageIndex, x, y_user,   y_system, cpuUserColour);
    drawLine(imageIndex, x, y_system, height,   cpuSystemColour);

    ++historyOffset;
    if (historyOffset == historyLength * 2)
        historyOffset = 0;

    update();
}

void RazorSysStatContent::memoryUpdate(float apps, float buffers, float cached)
{
    int imageIndex = historyOffset / historyLength;

    int x = historyOffset % historyLength;

    int height = historyImage[imageIndex].height() - titleFontPixelHeight;
    int y_apps    = height    - height * apps    + 0.5;
    int y_buffers = y_apps    - height * buffers + 0.5;
    int y_cached  = y_buffers - height * cached  + 0.5;

    drawLine(imageIndex, x, 0,         y_cached,  bgColour);
    drawLine(imageIndex, x, y_cached,  y_buffers, memCachedColour);
    drawLine(imageIndex, x, y_buffers, y_apps,    memBuffersColour);
    drawLine(imageIndex, x, y_apps,    height,    memAppsColour);

    ++historyOffset;
    if (historyOffset == historyLength * 2)
        historyOffset = 0;

    update();
}

void RazorSysStatContent::swapUpdate(float used)
{
    int imageIndex = historyOffset / historyLength;

    int x = historyOffset % historyLength;

    int height = historyImage[imageIndex].height() - titleFontPixelHeight;
    int y_used = height - height * used + 0.5;

    drawLine(imageIndex, x, 0,      y_used, bgColour);
    drawLine(imageIndex, x, y_used, height, swapUsedColour);

    ++historyOffset;
    if (historyOffset == historyLength * 2)
        historyOffset = 0;

    update();
}

void RazorSysStatContent::networkUpdate(unsigned received, unsigned transmitted)
{
    int imageIndex = historyOffset / historyLength;

    int x = historyOffset % historyLength;

    int height = historyImage[imageIndex].height() - titleFontPixelHeight;
    qreal min_value = qMin(qMax(static_cast<qreal>(qMin(received, transmitted)) / netRealMaximumSpeed, 0.0), 1.0);
    qreal max_value = qMin(qMax(static_cast<qreal>(qMax(received, transmitted)) / netRealMaximumSpeed, 0.0), 1.0);
    if (logarithmicScale)
    {
        min_value = qLn(min_value * (logScaleMax - 1.0) + 1.0) / qLn(2.0) / static_cast<qreal>(logScaleSteps);
        max_value = qLn(max_value * (logScaleMax - 1.0) + 1.0) / qLn(2.0) / static_cast<qreal>(logScaleSteps);
    }
    int y_min = height - height * min_value + 0.5;
    int y_max = height - height * max_value + 0.5;
    QColor maxColor = (received > transmitted) ? netReceivedColour : netTransmittedColour;

    drawLine(imageIndex, x, 0,     y_max,  bgColour);
    drawLine(imageIndex, x, y_max, y_min,  maxColor);
    drawLine(imageIndex, x, y_min, height, netBothColour);

    ++historyOffset;
    if (historyOffset == historyLength * 2)
        historyOffset = 0;

    update();

}

void RazorSysStatContent::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    int linesStart = 1;
    qreal graphTop = 0;
    qreal graphHeight = height();

    if (!titleLabel.isEmpty())
    {
        linesStart = 0;
        graphTop = titleFontPixelHeight - 1;
        graphHeight -= graphTop;
    }

    p.fillRect(rect(), bgColour);

    if (!titleLabel.isEmpty())
    {
        p.setPen(titleColour);
        p.setFont(titleFont);
        p.drawText(QRectF(0, 0, width(), graphTop), Qt::AlignHCenter | Qt::AlignVCenter, titleLabel);
    }


    if (historyOffset < historyLength)
    {
        int x = historyLength - historyOffset;
        int sx = 0;
        int sw = historyOffset;
        p.drawImage( x, titleFontPixelHeight, historyImage[0], sx, 0, sw, 0);
        p.drawImage(sx, titleFontPixelHeight, historyImage[1], sw, 0,  x, 0);
    }
    else
    {
        int x = 0;
        int sx = historyOffset - historyLength;
        int sw = historyLength - sx;
        p.drawImage( x, titleFontPixelHeight, historyImage[0], sx, 0, sw, 0);
        p.drawImage(sw, titleFontPixelHeight, historyImage[1],  x, 0, sx, 0);
    }


    p.setPen(gridColour);
    for (int l = linesStart; l <= gridLines; ++l)
    {
        qreal y = graphTop + l * 1.0 * graphHeight / (gridLines + 1);
        p.drawLine(QPointF(0, y), QPointF(width(), y));
    }
}
