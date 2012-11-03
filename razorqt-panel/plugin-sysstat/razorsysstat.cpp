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
    mContent(new RazorSysStatContent(*this, this))
{
    setObjectName("SysStat");

    QWidget *border = new QWidget(this);

    QVBoxLayout *borderLayout = new QVBoxLayout(border);
    borderLayout->setContentsMargins(0, 0, 0, 0);
    borderLayout->setSpacing(0);
    borderLayout->addWidget(mContent);

    addWidget(border);

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
    mContent->updateSettings(settings());
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

    updateInterval = settings.value("graph/updateInterval", 1.0).toDouble();
    historyLength = settings.value("graph/historyLength", 30).toInt();

    gridLines = settings.value("grid/lines", 1).toInt();
    gridColour = QColor(settings.value("grid/colour", "#c0c0c0").toString());

    titleLabel = settings.value("title/label", QString()).toString();
    QFont defaultFont(QApplication::font());
    titleFont = QFont(
        settings.value("title/font/family", defaultFont.family()).toString(),
        settings.value("title/font/pointSize", defaultFont.pointSize()).toInt(),
        settings.value("title/font/weight", defaultFont.weight()).toInt(),
        settings.value("title/font/italic", defaultFont.italic()).toBool() );
    titleColour = QColor(settings.value("title/colour", "#ffffff").toString());

    dataType = settings.value("data/type", QString("CPU")).toString();

    dataSource = settings.value("data/source", QString()).toString();

    cpuSystemColour = QColor(settings.value("cpu/systemColour",    "#800000").toString());
    cpuUserColour =   QColor(settings.value("cpu/userColour",      "#000080").toString());
    cpuNiceColour =   QColor(settings.value("cpu/niceColour",      "#008000").toString());
    cpuOtherColour =  QColor(settings.value("cpu/otherColour",     "#808000").toString());
    useFrequency = settings.value("cpu/useFrequency", true).toBool();
    frequencyColour = QColor(settings.value("cpu/frequencyColour", "#60c0c0").toString());

    memAppsColour =    QColor(settings.value("mem/appsColour",    "#000080").toString());
    memBuffersColour = QColor(settings.value("mem/buffersColour", "#008000").toString());
    memCachedColour =  QColor(settings.value("mem/cachedColour",  "#808000").toString());
    swapUsedColour =   QColor(settings.value("mem/swapColour",    "#800000").toString());

    netReceivedColour =    QColor(settings.value("net/receivedColour",    "#000080").toString());
    netTransmittedColour = QColor(settings.value("net/transmittedColour", "#808000").toString());
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
    historyImage[0] = QImage(historyLength, height(), QImage::Format_ARGB32);
    historyImage[1] = QImage(historyLength, height(), QImage::Format_ARGB32);
#if QT_VERSION < 0x040800
    historyImage[0].fill(QColor(Qt::transparent).rgba());
    historyImage[1].fill(QColor(Qt::transparent).rgba());
#else
    historyImage[0].fill(Qt::transparent);
    historyImage[1].fill(Qt::transparent);
#endif

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
    int y_system = qMin(qMax(height   - height * system * frequencyRate + 0.5, 0), height - 1);
    int y_user   = qMin(qMax(y_system - height * user   * frequencyRate + 0.5, 0), height - 1);
    int y_nice   = qMin(qMax(y_user   - height * nice   * frequencyRate + 0.5, 0), height - 1);
    int y_other  = qMin(qMax(y_nice   - height * other  * frequencyRate + 0.5, 0), height - 1);
    int y_freq   = qMin(qMax(height   - height *          frequencyRate + 0.5, 0), height - 1);

    drawLine(imageIndex, x, y_freq,   y_other,  frequencyColour);
    drawLine(imageIndex, x, y_other,  y_nice,   cpuOtherColour);
    drawLine(imageIndex, x, y_nice,   y_user,   cpuNiceColour);
    drawLine(imageIndex, x, y_user,   y_system, cpuUserColour);
    drawLine(imageIndex, x, y_system, height,   cpuSystemColour);

    ++historyOffset;
    if (historyOffset == historyLength * 2)
        historyOffset = 0;
    if (!(historyOffset % historyLength))
        historyImage[1 - imageIndex].fill(Qt::transparent);

    update();
}

void RazorSysStatContent::cpuUpdate(float user, float nice, float system, float other)
{
    int imageIndex = historyOffset / historyLength;

    int x = historyOffset % historyLength;

    int height = historyImage[imageIndex].height() - titleFontPixelHeight;
    int y_system = qMin(qMax(height   - height * system + 0.5, 0), height - 1);
    int y_user   = qMin(qMax(y_system - height * user   + 0.5, 0), height - 1);
    int y_nice   = qMin(qMax(y_user   - height * nice   + 0.5, 0), height - 1);
    int y_other  = qMin(qMax(y_nice   - height * other  + 0.5, 0), height - 1);

    drawLine(imageIndex, x, y_other,  y_nice,   cpuOtherColour);
    drawLine(imageIndex, x, y_nice,   y_user,   cpuNiceColour);
    drawLine(imageIndex, x, y_user,   y_system, cpuUserColour);
    drawLine(imageIndex, x, y_system, height,   cpuSystemColour);

    ++historyOffset;
    if (historyOffset == historyLength * 2)
        historyOffset = 0;
    if (!(historyOffset % historyLength))
        historyImage[1 - imageIndex].fill(Qt::transparent);

    update();
}

void RazorSysStatContent::memoryUpdate(float apps, float buffers, float cached)
{
    int imageIndex = historyOffset / historyLength;

    int x = historyOffset % historyLength;

    int height = historyImage[imageIndex].height() - titleFontPixelHeight;
    int y_apps    = qMin(qMax(height    - height * apps    + 0.5, 0), height - 1);
    int y_buffers = qMin(qMax(y_apps    - height * buffers + 0.5, 0), height - 1);
    int y_cached  = qMin(qMax(y_buffers - height * cached  + 0.5, 0), height - 1);

    drawLine(imageIndex, x, y_cached,  y_buffers, memCachedColour);
    drawLine(imageIndex, x, y_buffers, y_apps,    memBuffersColour);
    drawLine(imageIndex, x, y_apps,    height,    memAppsColour);

    ++historyOffset;
    if (historyOffset == historyLength * 2)
        historyOffset = 0;
    if (!(historyOffset % historyLength))
        historyImage[1 - imageIndex].fill(Qt::transparent);

    update();
}

void RazorSysStatContent::swapUpdate(float used)
{
    int imageIndex = historyOffset / historyLength;

    int x = historyOffset % historyLength;

    int height = historyImage[imageIndex].height() - titleFontPixelHeight;
    int y_used = qMin(qMax(height - height * used + 0.5, 0), height - 1);

    drawLine(imageIndex, x, y_used, height, swapUsedColour);

    ++historyOffset;
    if (historyOffset == historyLength * 2)
        historyOffset = 0;
    if (!(historyOffset % historyLength))
        historyImage[1 - imageIndex].fill(Qt::transparent);

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
    int y_min = qMin(qMax(height - height * min_value + 0.5, 0), height - 1);
    int y_max = qMin(qMax(height - height * max_value + 0.5, 0), height - 1);
    QColor maxColor = (received > transmitted) ? netReceivedColour : netTransmittedColour;

    drawLine(imageIndex, x, y_max, y_min,  maxColor);
    drawLine(imageIndex, x, y_min, height, netBothColour);

    ++historyOffset;
    if (historyOffset == historyLength * 2)
        historyOffset = 0;
    if (!(historyOffset % historyLength))
        historyImage[1 - imageIndex].fill(Qt::transparent);

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
