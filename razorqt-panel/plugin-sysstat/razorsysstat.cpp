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
    borderLayout->setStretchFactor(mContent, 1);

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
        confWindow = new RazorSysStatConfiguration(settings(), this);

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
    historyImage = QImage(historyLength, 100, QImage::Format_ARGB32);
#if QT_VERSION < 0x040800
    historyImage.fill(QColor(Qt::transparent).rgba());
#else
    historyImage.fill(Qt::transparent);
#endif

    update();
}

template <typename T>
T clamp(const T &value, const T &min, const T &max)
{
    return qMin(qMax(value, min), max);
}

// QPainter.drawLine with pen set to Qt::transparent doesn't clears anything
void RazorSysStatContent::clearLine(void)
{
    QRgb bg = QColor(Qt::transparent).rgba();
    for (int i = 0; i < 100; ++i)
        reinterpret_cast<QRgb*>(historyImage.scanLine(i))[historyOffset] = bg;
}

void RazorSysStatContent::cpuUpdate(float user, float nice, float system, float other, float frequencyRate, uint)
{
    int y_system = clamp(static_cast<int>(system * 100.0 * frequencyRate)           , 0, 99);
    int y_user   = clamp(static_cast<int>(user   * 100.0 * frequencyRate) + y_system, 0, 99);
    int y_nice   = clamp(static_cast<int>(nice   * 100.0 * frequencyRate) + y_user  , 0, 99);
    int y_other  = clamp(static_cast<int>(other  * 100.0 * frequencyRate) + y_nice  , 0, 99);
    int y_freq   = clamp(static_cast<int>(         100.0 * frequencyRate)           , 0, 99);

    clearLine();
    QPainter painter(&historyImage);
    if (y_system != 0)
    {
        painter.setPen(cpuSystemColour);
        painter.drawLine(historyOffset, y_system, historyOffset, 0);
    }
    if (y_user != y_system)
    {
        painter.setPen(cpuUserColour);
        painter.drawLine(historyOffset, y_user, historyOffset, y_system);
    }
    if (y_nice != y_user)
    {
        painter.setPen(cpuNiceColour);
        painter.drawLine(historyOffset, y_nice, historyOffset, y_user);
    }
    if (y_other != y_nice)
    {
        painter.setPen(cpuOtherColour);
        painter.drawLine(historyOffset, y_other, historyOffset, y_nice);
    }
    if (y_freq != y_other)
    {
        painter.setPen(frequencyColour);
        painter.drawLine(historyOffset, y_freq, historyOffset, y_other);
    }

    historyOffset = (historyOffset + 1) % historyLength;

    update();
}

void RazorSysStatContent::cpuUpdate(float user, float nice, float system, float other)
{
    int y_system = clamp(static_cast<int>(system * 100.0)           , 0, 99);
    int y_user   = clamp(static_cast<int>(user   * 100.0) + y_system, 0, 99);
    int y_nice   = clamp(static_cast<int>(nice   * 100.0) + y_user  , 0, 99);
    int y_other  = clamp(static_cast<int>(other  * 100.0) + y_nice  , 0, 99);

    clearLine();
    QPainter painter(&historyImage);
    if (y_system != 0)
    {
        painter.setPen(cpuSystemColour);
        painter.drawLine(historyOffset, y_system, historyOffset, 0);
    }
    if (y_user != y_system)
    {
        painter.setPen(cpuUserColour);
        painter.drawLine(historyOffset, y_user, historyOffset, y_system);
    }
    if (y_nice != y_user)
    {
        painter.setPen(cpuNiceColour);
        painter.drawLine(historyOffset, y_nice, historyOffset, y_user);
    }
    if (y_other != y_nice)
    {
        painter.setPen(cpuOtherColour);
        painter.drawLine(historyOffset, y_other, historyOffset, y_nice);
    }

    historyOffset = (historyOffset + 1) % historyLength;

    update();
}

void RazorSysStatContent::memoryUpdate(float apps, float buffers, float cached)
{
    int y_apps    = clamp(static_cast<int>(apps    * 100.0)            , 0, 99);
    int y_buffers = clamp(static_cast<int>(buffers * 100.0) + y_apps   , 0, 99);
    int y_cached  = clamp(static_cast<int>(cached  * 100.0) + y_buffers, 0, 99);

    clearLine();
    QPainter painter(&historyImage);
    if (y_apps != 0)
    {
        painter.setPen(memAppsColour);
        painter.drawLine(historyOffset, y_apps, historyOffset, 0);
    }
    if (y_buffers != y_apps)
    {
        painter.setPen(memBuffersColour);
        painter.drawLine(historyOffset, y_buffers, historyOffset, y_apps);
    }
    if (y_cached != y_buffers)
    {
        painter.setPen(memCachedColour);
        painter.drawLine(historyOffset, y_cached, historyOffset, y_buffers);
    }

    historyOffset = (historyOffset + 1) % historyLength;

    update();
}

void RazorSysStatContent::swapUpdate(float used)
{
    int y_used = clamp(static_cast<int>(used * 100.0), 0, 99);

    clearLine();
    QPainter painter(&historyImage);
    if (y_used != 0)
    {
        painter.setPen(swapUsedColour);
        painter.drawLine(historyOffset, y_used, historyOffset, 0);
    }

    historyOffset = (historyOffset + 1) % historyLength;

    update();
}

void RazorSysStatContent::networkUpdate(unsigned received, unsigned transmitted)
{
    qreal min_value = qMin(qMax(static_cast<qreal>(qMin(received, transmitted)) / netRealMaximumSpeed, 0.0), 1.0);
    qreal max_value = qMin(qMax(static_cast<qreal>(qMax(received, transmitted)) / netRealMaximumSpeed, 0.0), 1.0);
    if (logarithmicScale)
    {
        min_value = qLn(min_value * (logScaleMax - 1.0) + 1.0) / qLn(2.0) / static_cast<qreal>(logScaleSteps);
        max_value = qLn(max_value * (logScaleMax - 1.0) + 1.0) / qLn(2.0) / static_cast<qreal>(logScaleSteps);
    }

    int y_min_value = clamp(static_cast<int>(min_value * 100.0)              , 0, 99);
    int y_max_value = clamp(static_cast<int>(max_value * 100.0) + y_min_value, 0, 99);

    clearLine();
    QPainter painter(&historyImage);
    if (y_min_value != 0)
    {
        painter.setPen(netBothColour);
        painter.drawLine(historyOffset, y_min_value, historyOffset, 0);
    }
    if (y_max_value != y_min_value)
    {
        painter.setPen((received > transmitted) ? netReceivedColour : netTransmittedColour);
        painter.drawLine(historyOffset, y_max_value, historyOffset, y_min_value);
    }

    historyOffset = (historyOffset + 1) % historyLength;

    update();
}

void RazorSysStatContent::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    qreal graphTop = 0;
    qreal graphHeight = height();

    bool hasTitle = !titleLabel.isEmpty();

    if (hasTitle)
    {
        graphTop = titleFontPixelHeight - 1;
        graphHeight -= graphTop;

        if (event->region().intersects(QRect(0, 0, width(), graphTop)))
        {
            p.setPen(titleColour);
            p.setFont(titleFont);
            p.drawText(QRectF(0, 0, width(), graphTop), Qt::AlignHCenter | Qt::AlignVCenter, titleLabel);
        }
    }

    p.scale(1.0, -1.0);

    QRect targetRect(0, -height(), historyLength - historyOffset, graphHeight);
    if (event->region().intersects(targetRect))
        p.drawImage(targetRect, historyImage, QRect(historyOffset, 0, historyLength - historyOffset, 100));
    if (historyOffset)
    {
        targetRect = QRect(historyLength - historyOffset, -height(), historyOffset, graphHeight);
        if (event->region().intersects(targetRect))
            p.drawImage(targetRect, historyImage, QRect(0, 0, historyOffset, 100));
    }

    p.resetTransform();

    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(gridColour);
    qreal w = static_cast<qreal>(width());
    if (hasTitle)
        p.drawLine(QPointF(0.0, graphTop + 0.5), QPointF(w, graphTop + 0.5)); // 0.5 looks better with antialiasing
    for (int l = 0; l < gridLines; ++l)
    {
        qreal y = graphTop + static_cast<qreal>(l + 1) * graphHeight / (static_cast<qreal>(gridLines + 1));
        p.drawLine(QPointF(0.0, y), QPointF(w, y));
    }
}
